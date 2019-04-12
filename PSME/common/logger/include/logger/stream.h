/*!
 * @brief Logger stream C prototypes
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
 * @file stream.h
 */

#ifndef LOGGER_STREAM_H
#define LOGGER_STREAM_H

#include <stdint.h>

/*!
 * @enum logger_stream_type
 * @brief Logger stream type
 *
 * @var logger_stream_type::LOGGER_STREAM_STDOUT
 * Standard output stream
 *
 * @var logger_stream_type::LOGGER_STREAM_STDERR
 * Standard error output stream
 *
 * @var logger_stream_type::LOGGER_STREAM_FILE
 * File output stream
 *
 * @var logger_stream_type::LOGGER_STREAM_UDP
 * IP UDP output stream
 *
 * @var logger_stream_type::LOGGER_STREAM_DIRECT
 * log directly to the console (without queueing in the worker)
 * */
enum logger_stream_type {
    LOGGER_STREAM_STDOUT    = 0,
    LOGGER_STREAM_STDERR    = 1,
    LOGGER_STREAM_FILE      = 2,
    LOGGER_STREAM_UDP       = 3,
    LOGGER_STREAM_TCP       = 4,
    LOGGER_STREAM_DIRECT    = 5
};

struct logger_stream;
union logger_options;

/*!
 * @brief Dynamically create buffer for logger stream
 *
 * @param[in]   tag     Logger stream tag ID string
 * @param[in]   type    Give logger type to create
 * @param[in]   options Logger stream options. NULL means default options
 * @return      When success return dynamically allocated logger stream
 *              instance otherwise return NULL
 * */
struct logger_stream *logger_stream_create(enum logger_stream_type type,
        const char *tag, union logger_options *options);

/*!
 * @brief Destroy dynamically created buffer for logger stream
 *
 * @param[in]   inst    Logger stream instance to destroy
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_destroy(struct logger_stream *inst);

/*!
 * @brief Start logger stream
 *
 * @param[in]   inst    Logger stream instance
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_start(struct logger_stream *inst);

/*!
 * @brief Stop logger stream
 *
 * @param[in]   inst    Logger stream instance
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_stop(struct logger_stream *inst);

/*!
 * @brief Configure UDP stream with given IPv4 address and port number
 *
 * @param[in]   inst        Logger stream instance
 * @param[in]   ip_address  IPv4 address string like 127.0.0.1
 * @param[in]   port        UDP port number
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_open_udp(struct logger_stream *inst,
        const char *ip_address, uint16_t port);

/*!
 * @brief Configure TCP stream with given IPv4 address and port number
 *
 * @param[in]   inst        Logger stream instance
 * @param[in]   ip_address  IPv4 address string like 127.0.0.1
 * @param[in]   port        TCP port number
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_open_tcp(struct logger_stream *inst,
        const char *ip_address, uint16_t port);

/*!
 * @brief Configure file stream with given file name
 *
 * @param[in]   inst        Logger stream instance
 * @param[in]   file_name   File name string path like /var/log/example.log
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_open_file(struct logger_stream *inst,
        const char *file_name);

/*!
 * @brief Set logger options
 *
 * @param[in]   inst    Logger stream instance
 * @param[in]   options Options for logger stream. NULL means default
 * */
void logger_stream_set_options(struct logger_stream *inst,
        union logger_options *options);

/*!
 * @brief Get logger stream options
 *
 * @param[in]   inst    Logger stream instance
 * @param[out]  options Get logger stream options
 * */
void logger_stream_get_options(struct logger_stream *inst,
        union logger_options *options);

#endif /* LOGGER_STREAM_H */
