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
 * @file logger_stream_instance.h
 *
 * @brief Logger stream instance
 * */

#ifndef LOGGER_STREAM_INSTANCE_H
#define LOGGER_STREAM_INSTANCE_H

#include "logger/stream.h"
#include "logger_stream_message.h"

#include "threads.h"

/*! Stream handler */
typedef int (*stream_handler_t)(struct logger_stream *inst);

/*!
 * @struct logger_stream_handler
 * @brief Contains logger stream handlers for different actions based on
 * #logger_stream_type
 *
 * @var logger_stream_handler::create
 * Allocate buffers, create stream
 *
 * @var logger_stream_handler::destroy
 * Deallocate buffers, destroy stream
 *
 * @var logger_stream_handler::flush
 * Flush all data stored in buffers
 * */
struct logger_stream_handler {
    stream_handler_t create;
    stream_handler_t destroy;
    stream_handler_t flush;
};

/*!
 * @struct logger_stream
 * @brief Logger stream object
 *
 * @var logger_stream::buffer
 * Buffer that contains formatted log string to flush
 *
 * @var logger_stream::tag
 * Log tag string that will be add when #logger_options::option is
 * enabled
 *
 * @var logger_stream::handler
 * Stream handlers
 *
 * @var logger_stream::msg_list
 * Message list collected from #logger object for #logger_stream
 *
 * @var logger_stream::thread
 * Thread instance for stream
 *
 * @var logger_stream::cond
 * Conditional variable
 *
 * @var logger_stream::mutex
 * Mutex
 *
 * @var logger_stream::settings
 * Unique stream settings based on #logger_stream::type
 *
 * @var logger_stream::type
 * Unique stream type id
 *
 * @var logger_stream::index
 * Data index for #logger_stream::buffer
 *
 * @var logger_stream::buffer_size
 * Buffer size for #logger_stream::buffer
 *
 * @var logger_stream::options
 * Logger options
 *
 * @var logger_stream::is_running
 * Flag indicated that #logger_stream thread is running
 * */
struct logger_stream {
    char *buffer;
    const char *tag;
    void *settings;
    struct logger_stream_handler handler;
    struct logger_list msg_list;
    thrd_t thread;
    cnd_t cond;
    mtx_t mutex;
    enum logger_stream_type type;
    size_t index;
    size_t buffer_size;
    volatile union logger_options options;
    volatile bool is_running;
};

#endif /* LOGGER_STREAM_INSTANCE_H */
