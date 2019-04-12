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
 * @file logger_stream_standard.c
 *
 * @brief Logger stream standard implementation
 * */

#include "logger_stream_standard.h"

#include "logger_assert.h"
#include "logger_memory.h"

#include <stdio.h>
#include <string.h>

int logger_stream_standard_create(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    logger_memory_free(inst->buffer);
    /* Extend buffer size for null terminator character */
    inst->buffer = logger_memory_alloc(LOGGER_DEFAULT_STREAM_SIZE + 1);

    if (NULL != inst->buffer) {
        inst->buffer_size = LOGGER_DEFAULT_STREAM_SIZE;
    } else {
        inst->buffer_size = 0;
        return LOGGER_ERROR_NULL;
    }

    return LOGGER_SUCCESS;
}

int logger_stream_standard_destroy(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    logger_memory_free(inst->buffer);
    inst->buffer = NULL;
    inst->buffer_size = 0;

    return LOGGER_SUCCESS;
}

int logger_stream_standard_output_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    // fwrite blocks untill all data is written..
    fwrite(inst->buffer, 1, inst->index, stdout);
    fflush(stdout);

    return LOGGER_SUCCESS;
}

int logger_stream_standard_error_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    fwrite(inst->buffer, 1, inst->index, stderr);
    fflush(stderr);
    return LOGGER_SUCCESS;
}
