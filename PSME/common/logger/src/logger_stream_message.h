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
 * @file logger_stream_message.h
 *
 * @brief Logger stream message interface
 * */

#ifndef LOGGER_STREAM_MESSAGE_H
#define LOGGER_STREAM_MESSAGE_H

#include "logger/logger.h"
#include "logger_time.h"
#include "logger_list.h"

/*!
 * @enum logger_stream_message_type
 * @brief Logger stream message type
 *
 * @var logger_stream_message_type::LOGGER_MESSAGE_OPEN
 * Open output stream
 *
 * @var logger_stream_message_type::LOGGER_MESSAGE_STREAM_WRITE
 * Log stream message
 *
 * @var logger_stream_message_type::LOGGER_MESSAGE_STREAM_FLUSH
 * Flush out all data stored in buffer stream
 * */
enum logger_stream_message_type {
    LOGGER_MESSAGE_OPEN,
    LOGGER_MESSAGE_STREAM_WRITE,
    LOGGER_MESSAGE_STREAM_FLUSH
};

/*!
 * @struct logger_stream_message
 * @brief Logger stream message object
 *
 * @var logger_stream_message::file_name
 * File name string
 *
 * @var logger_stream_message::function_name
 * Function name string
 *
 * @var logger_stream_message::tag
 * Log tag string
 *
 * @var logger_stream_message::stream_list
 * List of streams that message will be write
 *
 * @var logger_stream_message::line_number
 * File line number
 *
 * @var logger_stream_message::options
 * Options
 *
 * @var logger_stream_message::log_time
 * Log time stamp
 *
 * @var logger_stream_message::message
 * Log message
 * */
struct logger_stream_message {
    const char *file_name;
    const char *function_name;
    const char *tag;
    struct logger_list stream_list;
    unsigned int line_number;
    union logger_options options;
    struct logger_time log_time;
    char message[];
};

/*!
 * @brief Add message to logger stream queue. Private method for library
 *
 * @param[in]   inst        Logger stream instance
 * @param[in]   msg         Message instance to add
 * @param[in]   type        Message type
 * @return      When success return #LOGGER_SUCCESS otherwise a negative
 *              error code
 * */
int logger_stream_add_message(struct logger_stream *inst,
        void *msg, enum logger_stream_message_type type);

#endif /* LOGGER_STREAM_MESSAGE_H */
