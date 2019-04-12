/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file logger_buffer.c
 *
 * @brief Logger buffer implementation
 * */

#include "logger/buffer.h"

#include "logger_assert.h"
#include "logger_memory.h"

#include <safe-string/safe_lib.h>
#include <stdio.h>
#include <string.h>

/*!
 * @struct logger_buffer
 * @brief Logger buffer object
 *
 * @var logger_buffer::index
 * Data index in message buffer
 *
 * @var logger_buffer::size
 * Maximum size for message buffer
 *
 * @var logger_buffer::message
 * Message buffer
 * */
struct logger_buffer {
    char *message;
    size_t index;
    size_t size;
};

static int logger_buffer_resize(
        struct logger_buffer *buffer, size_t size) {
    logger_assert(NULL != buffer);

    char *msg = logger_memory_alloc(buffer->size + size);
    if (NULL == msg) {
        return LOGGER_ERROR_MEMORY_OUT;
    }

    memcpy_s(msg, buffer->size + size, buffer->message, buffer->index + 1);
    logger_memory_free(buffer->message);
    buffer->size += size;
    buffer->message = msg;

    return LOGGER_SUCCESS;
}

void logger_buffer_write(struct logger_buffer *buffer, const char *message) {
    if ((NULL == buffer) || (NULL == buffer->message)) return;

    size_t message_length = strnlen_s(message, RSIZE_MAX_STR);
    size_t total_length = buffer->index + message_length + 1;

    if (total_length > buffer->size) {
        if (logger_buffer_resize(buffer, LOGGER_DEFAULT_BUFFER_SIZE +
                    total_length) != LOGGER_SUCCESS) return;
    }

    memcpy_s(&buffer->message[buffer->index], message_length, message, message_length);
    buffer->index += message_length;
    buffer->message[buffer->index] = '\0';
}

LOGGER_PRINTF_FORMAT(2, 0)
void logger_buffer_vwrite(struct logger_buffer *buffer,
        const char *fmt, va_list args) {
    if ((NULL == buffer) || (NULL == buffer->message)) return;

    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0) {
        return;
    }

    char *msg_copy = logger_memory_alloc((size_t)size + 1);
    if (NULL == msg_copy) {
        return;
    }

    vsnprintf(msg_copy, (size_t)size, fmt, args);

    logger_buffer_write(buffer, msg_copy);

    logger_memory_free(msg_copy);
}

struct logger_buffer *logger_buffer_create(void) {
    struct logger_buffer *buffer =
        logger_memory_alloc(sizeof(struct logger_buffer));
    if (NULL == buffer) {
        return NULL;
    }

    buffer->message = logger_memory_alloc(LOGGER_DEFAULT_BUFFER_SIZE);
    if (NULL == buffer->message) {
        logger_memory_free(buffer);
        return NULL;
    }

    buffer->size = LOGGER_DEFAULT_BUFFER_SIZE;
    buffer->message[0] = '\0';
    buffer->index = 0;

    return buffer;
}

void logger_buffer_clear(struct logger_buffer *buffer) {
    if (NULL == buffer) return;

    buffer->message[0] = '\0';
    buffer->index = 0;
}

LOGGER_PRINTF_FORMAT(2, 0)
void logger_buffer_fwrite(struct logger_buffer *buffer, const char *fmt, ...) {
    if ((NULL == buffer) || (NULL == buffer->message)) return;

    va_list args;
    va_start(args, fmt);

    logger_buffer_vwrite(buffer, fmt, args);

    va_end(args);
}

void logger_buffer_vector(struct logger_buffer *buffer,
        const void *data, unsigned int size) {
    if ((NULL == buffer) || (NULL == buffer->message)) return;

    const char *pdata = (const char *)data;

    logger_buffer_fwrite(buffer, "[%d][", size);
    if (size > 0) {
        logger_buffer_fwrite(buffer, "%02X", 0xFF & pdata[0]);
    }
    for (size_t i = 1; i < size; i++) {
        logger_buffer_fwrite(buffer, " %02X", 0xFF & pdata[i]);
    }
    logger_buffer_write(buffer, "]");
}

const char *logger_buffer_string(struct logger_buffer *buffer) {
    if ((NULL == buffer) || (NULL == buffer->message)) return "";

    return buffer->message;
}

void logger_buffer_destroy(struct logger_buffer *buffer) {
    if (NULL == buffer) return;

    logger_memory_free(buffer->message);
    logger_memory_free(buffer);
}
