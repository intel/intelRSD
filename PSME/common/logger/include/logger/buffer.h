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
 * @file logger_buffer.h
 *
 * @brief Logger buffer interface
 * */

#ifndef LOGGER_BUFFER_H
#define LOGGER_BUFFER_H

#include "logger/logger.h"

struct logger_buffer;

/*!
 * @brief Create dynamically allocated buffer with initial default size.
 * User must provide to destroy allocated buffer with logger_buffer_destroy
 * after use
 *
 * @return                  When success return dynamically allocated buffer
 *                          instance otherwise return NULL
 */
struct logger_buffer *logger_buffer_create(void);

/*!
 * @brief It will clean buffer setting null-terminator at the begin.
 * It may be used many times after when buffer was created to clean it.
 * Doesn't required to use after buffer create function
 *
 * @param[in]   inst    Buffer instance
 * */
void logger_buffer_clear(struct logger_buffer *inst);

/*!
 * @brief Write log to buffer. Good to split and join log information.
 *
 * @param[in]   inst    Buffer instance
 * @param[in]   fmt     Format string like printf
 * @param[in]   ...     Variadic variables like printf
 * */
void logger_buffer_fwrite(struct logger_buffer *inst, const char *fmt, ...);

/*!
 * @brief Write log to buffer. Good to split and join log information.
 *
 * @param[in]   inst    Buffer instance
 * @param[in]   fmt     Format string like printf
 * @param[in]   args    Variadic variables like vprintf
 * */
void logger_buffer_vwrite(struct logger_buffer *inst,
        const char *fmt, va_list args) LOGGER_PRINTF_FORMAT(2, 0);

/*!
 * @brief Write vector to buffer. Good to split and join log information.
 * Format: [size][AA BB CC DD ...]
 *
 * @param[in]   inst    Buffer instance
 * @param[in]   data    Data array/memory pointer
 * @param[in]   size    Data array/memory size
 * */
void logger_buffer_vector(struct logger_buffer *inst,
        const void *data, unsigned int size);

/*!
 * @brief Destroy dynamically allocated buffer
 *
 * @param[in]   inst    Buffer instance
 * */
void logger_buffer_destroy(struct logger_buffer *inst);

/*!
 * @brief Write log to buffer. Good to split and join log information.
 *
 * @param[in]   inst    Buffer instance
 * @param[in]   message Message string
 * */
void logger_buffer_write(struct logger_buffer *inst, const char *message);

/*!
 * @brief Get string
 *
 * @param[in]   inst    Buffer instance
 * @return              String message from buffer
 */
const char *logger_buffer_string(struct logger_buffer *inst);

/*!
 * @brief Logger
 *
 * @param[in]       inst Buffer instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define logbuf_write(inst, ...)\
    logger_buffer_fwrite((inst), __VA_ARGS__)

/*!
 * @brief Print vector in format: [size][HEX]
 *
 * @param[in]       inst Buffer instance
 * @param[in]       data Data vector/array pointer
 * @param[in]       size Data vector/array size
 * */
#define logbuf_vector(inst, data, size)\
    logger_buffer_vector((inst), (data), (size))

/*!
 * @brief Print array in format: [size][HEX]
 *
 * @param[in]       inst Buffer instance
 * @param[in]       data Data vector/array pointer
 * @param[in]       size Data vector/array size
 * */
#define logbuf_array(inst, data, size)\
    logger_buffer_vector((inst), (data), (size))

/*!
 * @brief Create buffer
 *
 * @return          When success return created logger buffer object otherwise
 *                  return NULL
 * */
#define logbuf_create()\
    logger_buffer_create()

/*!
 * @brief Destroy buffer
 *
 * @param[in]       inst Buffer instance
 * */
#define logbuf_destroy(inst)\
    logger_buffer_destroy((inst))

/*!
 * @brief Clear message
 *
 * @param[in]       inst Buffer instance
 * */
#define logbuf_clear(inst)\
    logger_buffer_clear((inst))

/*!
 * @brief Get string from buffer instance
 *
 * @param[in]   inst    Buffer instance
 * @return              Always return string, if buffer is not valid it will
 *                      return empty string ""
 * */
#define logbuf_string(inst)\
    logger_buffer_string((inst))

#endif /* LOGGER_BUFFER_H */
