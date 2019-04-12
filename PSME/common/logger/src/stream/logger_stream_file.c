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
 * @file logger_stream_file.c
 *
 * @brief Logger stream file implementation
 * */

#include "logger_stream_file.h"

#include "logger_alloc.h"
#include "logger_assert.h"
#include "logger_memory.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/*!
 * @def LOGGER_FILE_MODE
 * File mode
 *
 * @def LOGGER_FILE_PRIV
 * File privileges setting after creating file
 * */
#define LOGGER_FILE_MODE    (O_CREAT | O_WRONLY | O_APPEND | O_SYNC)
#define LOGGER_FILE_PRIV    (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

/*!
 * @struct logger_stream_setting_file
 * @brief Settings for file stream
 *
 * @var logger_stream_setting_file::file_name
 * File name path string point to file that will be create by
 * #logger_stream_file_create when file doesn't exist. All flushed data
 * will be write to that file by #logger_stream_file_flush
 * */
struct logger_stream_setting_file {
    char *file_name;
};

int logger_stream_open_file(struct logger_stream *inst,
        const char *file_name) {
    logger_assert(NULL != inst);
    if (LOGGER_STREAM_FILE != inst->type) return LOGGER_ERROR_TYPE;

    int err;
    struct logger_stream_setting_file *file =
        logger_memory_alloc(sizeof(struct logger_stream_setting_file));

    if (NULL == file) {
        return LOGGER_ERROR_NULL;
    }

    file->file_name = logger_alloc_copy_string(file_name);

    if (NULL == file->file_name) {
        logger_memory_free(file);
        return LOGGER_ERROR_NULL;
    }

    err = logger_stream_add_message(inst, file, LOGGER_MESSAGE_OPEN);
    if (LOGGER_SUCCESS != err) {
        logger_memory_free(file->file_name);
        logger_memory_free(file);
        return err;
    }

    return LOGGER_SUCCESS;
}

int logger_stream_file_create(struct logger_stream *inst) {
    logger_assert(NULL != inst);
    if (NULL == inst->settings) return LOGGER_ERROR_NULL;

    struct logger_stream_setting_file *file = inst->settings;

    if (NULL == file->file_name) {
        return LOGGER_ERROR_NULL;
    }

    int err;
    size_t buffer_size;
    struct stat file_stat;

    int fd = open(file->file_name, LOGGER_FILE_MODE, LOGGER_FILE_PRIV);
    if (fd < 0) {
        return fd;
    }
    err = fstat(fd, &file_stat);
    close(fd);

    /* fstat failed */
    if (err) {
        return err;
    }

    logger_memory_free(inst->buffer);

    if (file_stat.st_blksize > 0) {
        buffer_size = (size_t)file_stat.st_blksize;
    } else {
        buffer_size = LOGGER_DEFAULT_STREAM_SIZE;
    }

    inst->buffer = logger_memory_alloc(buffer_size);
    inst->buffer_size = buffer_size;

    if (NULL == inst->buffer) {
        inst->buffer = NULL;
        inst->buffer_size = 0;
        return LOGGER_ERROR_NULL;
    }

    return LOGGER_SUCCESS;
}

int logger_stream_file_destroy(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    struct logger_stream_setting_file *file = inst->settings;

    if (NULL != file) {
        logger_memory_free(file->file_name);
        file->file_name = NULL;
    }
    logger_memory_free(file);
    inst->settings = NULL;

    logger_memory_free(inst->buffer);
    inst->buffer = NULL;
    inst->buffer_size = 0;

    return LOGGER_SUCCESS;
}

int logger_stream_file_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);
    if (NULL == inst->settings) return LOGGER_ERROR_NULL;;

    struct logger_stream_setting_file *file = inst->settings;

    ssize_t err;

    if (NULL == file->file_name) {
        return LOGGER_ERROR_NULL;
    }

    int fd = open(file->file_name, LOGGER_FILE_MODE, LOGGER_FILE_PRIV);
    if (fd < 0) {
        return LOGGER_ERROR;
    }

    err = write(fd, inst->buffer, inst->index);

    close(fd);

    if (err < 0) {
        return (int)err;
    }

    return LOGGER_SUCCESS;
}
