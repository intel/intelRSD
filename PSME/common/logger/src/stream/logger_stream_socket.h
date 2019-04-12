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
 * @file logger_stream_socket.h
 *
 * @brief Logger stream socket interface
 */

#ifndef LOGGER_STREAM_SOCKET_H
#define LOGGER_STREAM_SOCKET_H

#include "logger_stream_config.h"

/*!
 * @brief Create socket stream
 *
 * @param[in] inst  Logger stream instance
 * @return          When success return #LOGGER_SUCCESS, otherwise a negative
 *                  error code
 * */
int logger_stream_socket_create(struct logger_stream *inst);

/*!
 * @brief Destroy socket stream
 *
 * @param[in] inst  Logger stream instance
 * @return          When success return #LOGGER_SUCCESS, otherwise a negative
 *                  error code
 * */
int logger_stream_socket_destroy(struct logger_stream *inst);

/*!
 * @brief Flush all data stored in UDP socket stream
 *
 * @param[in] inst  Logger stream instance
 * @return          When success return #LOGGER_SUCCESS, otherwise a negative
 *                  error code
 * */
int logger_stream_socket_udp_flush(struct logger_stream *inst);

/*!
 * @brief Flush all data stored in TCP socket stream
 *
 * @param[in] inst  Logger stream instance
 * @return          When success return #LOGGER_SUCCESS, otherwise a negative
 *                  error code
 * */
int logger_stream_socket_tcp_flush(struct logger_stream *inst);

#endif /* LOGGER_STREAM_SOCKET_H */
