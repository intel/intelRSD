/*!
 * @brief Logger stream implementation
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file logger_stream.c
 */

#include "logger/stream.h"
#include "logger_stream_instance.h"
#include "stream/logger_stream_config.h"

#include "logger_assert.h"
#include "logger_alloc.h"
#include "logger_color.h"
#include "logger_level.h"
#include "logger_memory.h"

#include <safe-string/safe_lib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*!
 * @def LOGGER_TIME_SIZE
 * Maximum buffer size for formatted time string
 *
 * @def LOGGER_BUFFER_SIZE
 * Maximum buffer size for formatted string
 * */
#define LOGGER_TIME_SIZE                20
#define LOGGER_BUFFER_SIZE              80

/*!
 * @def buffer_write(_dst, _src)
 * Copy data to current buffer position and change current buffer position
 * to new location pointed after copied data
 *
 * @def buffer_color(_dst, _options, _color)
 * when color option is enabled, write color ASCI string to buffer
 * */
#define buffer_write(_dst, _src)\
    do {\
        size_t _str_size = strnlen_s(_src, RSIZE_MAX_STR);\
        size_t _buffer_used = (size_t)(_dst - buffer);\
        logger_assert(_buffer_used < LOGGER_BUFFER_SIZE);\
        memcpy_s(_dst, (rsize_t)(LOGGER_BUFFER_SIZE - _buffer_used), _src, _str_size);\
        _dst += _str_size;\
    } while (0)

#define buffer_color(_dst, _options, _color)\
    do{\
        if (options.option.color) {\
            buffer_write(_dst, _color);\
        }\
    }while(0)

static const struct timespec g_wait_time = {
    .tv_sec = LOGGER_DEFAULT_THREAD_WAKE_SEC,
    .tv_nsec = 0
};

static struct timespec g_wake_up_time = {
    .tv_sec = 0,
    .tv_nsec = 0
};

static const union logger_options g_logger_stream_default_options = {
    .option = {
        .level = LOG_DEBUG,
        .time_format = LOG_TIME_DATE_NS,
        .color = true,
        .tagging = true,
        .more_debug = true,
        .output_enable = true
    }
};

static bool message_compare(void *obj1, int id1, void *obj2, int id2) {
    if (LOGGER_MESSAGE_STREAM_WRITE != id2) {
        return false;
    }

    if (LOGGER_MESSAGE_STREAM_WRITE != id1) {
        return true;
    }

    logger_assert(NULL != obj1);
    logger_assert(NULL != obj2);

    struct logger_stream_message *msg1 = obj1;
    struct logger_stream_message *msg2 = obj2;

    return logger_time_compare(&msg1->log_time, &msg2->log_time) <= 0;
}

static inline bool logger_is_newline_terminated(const char *str);

static inline void logger_stream_set_color(struct logger_stream *const inst,
        union logger_options *options,
        const char *const color);

static void* logger_stream_task(void *pobj);

static void logger_stream_message_handle(struct logger_stream *inst,
        void *object, int id);

static int logger_stream_write(struct logger_stream *inst,
        const char *data, const size_t size);

static void logger_stream_write_string(struct logger_stream *const inst,
        const char *const str);

static void logger_stream_write_message(struct logger_stream *inst,
        struct logger_stream_message *msg);

static int logger_stream_flush(struct logger_stream *inst);

struct logger_stream *logger_stream_create(enum logger_stream_type type,
        const char* tag, union logger_options *options) {

    struct logger_stream *inst =
        logger_memory_alloc(sizeof(struct logger_stream));

    if (NULL != inst) {
        int err;

        memset(inst, 0, sizeof(struct logger_stream));
        const struct logger_stream_handler *handler =
            logger_stream_config_by_type(type);

        if (NULL != options) {
            inst->options.raw = options->raw;
        } else {
            inst->options.raw = g_logger_stream_default_options.raw;
        }
        inst->tag = tag;

        if (NULL != handler) {
            inst->type = type;
            inst->handler = *handler;
        } else {
            logger_memory_free(inst);
            return NULL;
        }

        err = mtx_init(&inst->mutex, mtx_plain);
        if (thrd_success != err) {
            logger_memory_free(inst);
            return NULL;
        }

        err = cnd_init(&inst->cond);
        if (thrd_success != err) {
            mtx_destroy(&inst->mutex);
            logger_memory_free(inst);
            return NULL;
        }

        if (NULL != inst->handler.create) {
            inst->handler.create(inst);
        }

        err = logger_stream_start(inst);
        if (thrd_success != err) {
            mtx_destroy(&inst->mutex);
            cnd_destroy(&inst->cond);
            logger_memory_free(inst);
            return NULL;
        }
    }

    return inst;
}

int logger_stream_destroy(struct logger_stream *inst) {
    if (NULL != inst) {
        int err;
        int msg_id;
        void *msg_object;

        err = logger_stream_stop(inst);
        if (LOGGER_SUCCESS != err) {
            return err;
        }

        struct logger_list msg_list = LOGGER_LIST_INIT;

        logger_list_move(&msg_list, &inst->msg_list);

        while (!logger_list_empty(&msg_list)) {
            msg_object = logger_list_pop(&msg_list, &msg_id);
            logger_stream_message_handle(inst, msg_object, msg_id);
        }

        logger_stream_flush(inst);
        mtx_destroy(&inst->mutex);
        cnd_destroy(&inst->cond);

        if (NULL != inst->handler.destroy) {
            inst->handler.destroy(inst);
        }

        logger_memory_free(inst);
    }

    return LOGGER_SUCCESS;
}

int logger_stream_start(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    if (inst->is_running) {
        return LOGGER_SUCCESS;
    }

    int err;

    err = thrd_create(&inst->thread, logger_stream_task, inst);
    if (thrd_success != err) {
        return err;
    }

    while(false == inst->is_running);

    return LOGGER_SUCCESS;
}

int logger_stream_stop(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    if (false == inst->is_running) {
        return LOGGER_SUCCESS;
    }

    inst->is_running = false;

    int err;

    err = thrd_join(inst->thread, NULL);
    if (thrd_success != err) {
        return err;
    }

    return LOGGER_SUCCESS;
}

int logger_stream_add_message(struct logger_stream *inst,
        void *msg, enum logger_stream_message_type type) {
    logger_assert(NULL != inst);

    int err;

    /* Enter thread safe section */
    err = mtx_lock(&inst->mutex);
    if (thrd_success != err) {
        return err;
    }

    if (inst->type == LOGGER_STREAM_DIRECT) {
        // log message without ANY buffering in the logger
        logger_stream_write_message(inst, msg);
        logger_memory_free(msg);
        logger_stream_flush(inst);
    } else {
        /* Push log message to queue */
        err = logger_list_push(&inst->msg_list, msg, type);
        if (thrd_success != err) {
            mtx_unlock(&inst->mutex);
            return err;
        }
    }

    /* Wake-up thread */
    cnd_signal(&inst->cond);

    /* Leave thread safe section */
    mtx_unlock(&inst->mutex);

    return LOGGER_SUCCESS;
}

static inline bool logger_is_newline_terminated(const char *str) {
    if (NULL == str) return false;

    size_t length = strnlen_s(str, RSIZE_MAX_STR);
    return (length > 0) && (str[length - 1] == '\n');
}

static inline void logger_stream_set_color(struct logger_stream *const inst,
        union logger_options *options,
        const char *const color) {
    if (options->option.color) {
        logger_stream_write_string(inst, color);
    }
}

static void logger_stream_write_message(struct logger_stream *inst,
        struct logger_stream_message *msg) {
    logger_assert(NULL != inst);
    logger_assert(NULL != msg);

    struct tm *timeval;
    time_t time_seconds;
    long int time_nanoseconds;
    int terminated;

    union logger_options options = {.raw = 0};
    options.raw = inst->options.raw & msg->options.raw;

    /* Write log time stamp to the stream */
    char buffer[LOGGER_BUFFER_SIZE];
    char *buffer_ptr = buffer;

    logger_time_get(&msg->log_time, &time_seconds, &time_nanoseconds);
    timeval = localtime(&time_seconds);
    if (NULL != timeval) {
        buffer_color(buffer_ptr, options, COLOR_GREEN_NORMAL);

        buffer_ptr += strftime(buffer_ptr, LOGGER_TIME_SIZE, "%F %T", timeval);
        buffer_ptr += snprintf(buffer_ptr,
            (size_t) (LOGGER_BUFFER_SIZE - (buffer_ptr - buffer)),
            ".%09lu", time_nanoseconds);

        switch (inst->options.option.time_format) {
            case LOG_TIME_DATE_SEC:
                buffer_ptr -= 4; // fallthrough
            case LOG_TIME_DATE_MS:
                buffer_ptr -= 3; // fallthrough
            case LOG_TIME_DATE_US:
                buffer_ptr -= 3; // fallthrough
            case LOG_TIME_DATE_NS:
                buffer_color(buffer_ptr, options, COLOR_DEFAULT);
                buffer_write(buffer_ptr, " - ");
                break;
            case LOG_TIME_NONE:
                /* No time stamp message */
                buffer_ptr = buffer;
                break;
            default:
                buffer_ptr = buffer;
                buffer_color(buffer_ptr, options, COLOR_DEFAULT);
                buffer_write(buffer_ptr, "unsupported - ");
                break;
        }
    } else {
        buffer_ptr = buffer;
        buffer_color(buffer_ptr, options, COLOR_DEFAULT);
        buffer_write(buffer_ptr, "no time - ");
    }

    /* Write log level tag to the stream */
    buffer_color(buffer_ptr, options, logger_color_by_level(msg->options.option.level));
    buffer_write(buffer_ptr, logger_level_get_string(msg->options.option.level));

    buffer_color(buffer_ptr, options, COLOR_DEFAULT);
    buffer_write(buffer_ptr, " - ");
    buffer_color(buffer_ptr, options, COLOR_CYAN_NORMAL);

    logger_stream_write(inst, buffer, (size_t)(buffer_ptr - buffer));

    /* Write message ID tag */
    if (options.option.tagging) {
        if (NULL != msg->tag) {
            logger_stream_write_string(inst, msg->tag);

            logger_stream_set_color(inst, &options, COLOR_DEFAULT);
            logger_stream_write_string(inst, " - ");
        }
    }

    terminated = logger_is_newline_terminated(msg->message);
    if (!terminated) {
        logger_stream_write_string(inst, msg->message);
    }

    /* Write more debug information to the stream */
    if (options.option.more_debug) {
        char *debug_buffer = NULL;
        int size = snprintf(NULL, 0, "[%s:%s:%d]",
                msg->file_name,
                msg->function_name,
                msg->line_number);
        if (size > 0) {
            if (!terminated) {
                logger_stream_write_string(inst, " ");
            }

            logger_stream_set_color(inst, &options, COLOR_YELLOW_NORMAL);

            size++;
            debug_buffer = logger_memory_alloc((size_t)size);
            if (NULL != debug_buffer) {
                snprintf(debug_buffer, (size_t)size, "[%s:%s:%d]",
                         msg->file_name,
                         msg->function_name,
                         msg->line_number);
                logger_stream_write_string(inst, debug_buffer);
                logger_memory_free(debug_buffer);
            }
        }
        logger_stream_set_color(inst, &options, COLOR_DEFAULT);
    }

    if (options.option.more_debug && terminated) {
        logger_stream_write_string(inst, "\n");
        logger_stream_write_string(inst, msg->message);
    }
    else if (!terminated) {
        logger_stream_write_string(inst, "\n");
    }
    else {
        logger_stream_write_string(inst, msg->message);
    }
}

static int logger_stream_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    if ((NULL == inst->buffer) || (0 == inst->buffer_size)) {
        return LOGGER_SUCCESS;
    }

    int err;

    if (NULL != inst->handler.flush) {
        err = inst->handler.flush(inst);
        if (LOGGER_SUCCESS != err) {
            inst->index = 0;
            return err;
        }
    }
    inst->index = 0;

    return LOGGER_SUCCESS;
}

static int logger_stream_write(struct logger_stream *inst,
        const char *data, const size_t size) {
    logger_assert(NULL != inst);

    if ((NULL == inst->buffer) || (0 == inst->buffer_size)) {
        return LOGGER_SUCCESS;
    }

    if ((NULL == data) || (0 == size)) {
        return LOGGER_SUCCESS;
    }

    int err;
    size_t space = inst->buffer_size - inst->index;

    if (size < space) {
        /* Write all chars to stream */
        memcpy_s(&inst->buffer[inst->index], space, data, size);
        inst->index += size;
    } else if (size == space) {
        /* Write all chars to stream and flush to disk */
        memcpy_s(&inst->buffer[inst->index], space, data, size);
        inst->index += size;

        err = logger_stream_flush(inst);
        if (LOGGER_SUCCESS != err) {
            return err;
        }
    } else {
        /* Write some chars to fill up stream and flush to disk */
        memcpy_s(&inst->buffer[inst->index], space, data, space);
        inst->index += space;

        err = logger_stream_flush(inst);
        if (LOGGER_SUCCESS != err) {
            return err;
        }

        err = logger_stream_write(inst, &data[space], size - space);
        if (LOGGER_SUCCESS != err) {
            return err;
        }
    }

    return LOGGER_SUCCESS;
}

static void logger_stream_write_string(struct logger_stream *const inst,
        const char *const str) {
    logger_assert(NULL != inst);

    logger_stream_write(inst, str, strnlen_s(str, RSIZE_MAX_STR));
}

static void logger_stream_message_handle(struct logger_stream *inst,
        void *object, int id) {

    switch(id) {
    case LOGGER_MESSAGE_OPEN:
        logger_stream_flush(inst);

        if (NULL != inst->handler.destroy) {
            inst->handler.destroy(inst);
        }

        inst->settings = object;

        if (NULL != inst->handler.create) {
            inst->handler.create(inst);
        }
        break;
    case LOGGER_MESSAGE_STREAM_WRITE:
        logger_stream_write_message(inst, object);
        logger_memory_free(object);
        break;
    case LOGGER_MESSAGE_STREAM_FLUSH:
        logger_stream_flush(inst);
        break;
    default:
        break;
    }
}

static void logger_stream_wait_for_signal(struct logger_stream *inst) {
    /* Auto wake-up */

    time(&g_wake_up_time.tv_sec);
    g_wake_up_time.tv_sec += g_wait_time.tv_sec;

    int err = cnd_timedwait(&inst->cond, &inst->mutex, &g_wake_up_time);
    if (thrd_timedout == err) {
        logger_list_push(&inst->msg_list,
                NULL, LOGGER_MESSAGE_STREAM_FLUSH);
    }
}

static void* logger_stream_task(void *pobj) {
    logger_assert(NULL != pobj);

    int msg_id = 0;
    void *msg_object = NULL;
    struct logger_stream *inst = pobj;
    struct logger_list msg_list = LOGGER_LIST_INIT;

    inst->is_running = true;
    while (inst->is_running) {
        mtx_lock(&inst->mutex);
        /* Wait for signal (or timeout) that will wake-up this thread */
        if (logger_list_empty(&inst->msg_list)) {
            /* Thread go sleep only when there is no jobs to do */
            logger_stream_wait_for_signal(inst);
        }
        /* Move background list to foreground list. Thread safe */
        logger_list_move(&msg_list, &inst->msg_list);

        mtx_unlock(&inst->mutex);

        /* Sort list */
        logger_list_sort(&msg_list, message_compare);

        /* Process all messages in foreground list */
        while (!logger_list_empty(&msg_list)) {
            /* Get message from queue */
            msg_object = logger_list_pop(&msg_list, &msg_id);
            /* Create log message and write to the stream */
            logger_stream_message_handle(inst, msg_object, msg_id);
        }
    }

    /* Before exit, flush all logs */
    logger_stream_flush(inst);

    return 0;
}

void logger_stream_set_options(struct logger_stream *inst,
        union logger_options *options) {
    logger_assert(NULL != inst);

    if (NULL != options) {
        inst->options.raw = options->raw;
    } else {
        inst->options.raw = g_logger_stream_default_options.raw;
    }
}

void logger_stream_get_options(struct logger_stream *inst,
        union logger_options *options) {
    logger_assert(NULL != inst);

    if (NULL != options) {
        options->raw = inst->options.raw;
    }
}
