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
 * @file logger_stream_config.c
 *
 * @brief Logger stream configuration implementation
 * */

#include "logger_stream_config.h"

#include "logger_stream_standard.h"
#include "logger_stream_file.h"
#include "logger_stream_socket.h"

/*!
 * @struct logger_stream_config
 * @brief Logger stream configuration object
 *
 * @var logger_stream_config::type
 * Stream type
 *
 * @var logger_stream_config::handler
 * Stream handlers
 * */
struct logger_stream_config {
    enum logger_stream_type type;           /*!< Stream type */
    struct logger_stream_handler handler;   /*!< Stream handlers */
};

static const struct logger_stream_config gconfig[] = {
    {
        .type = LOGGER_STREAM_STDOUT,
        .handler = {
            logger_stream_standard_create,
            logger_stream_standard_destroy,
            logger_stream_standard_output_flush
        }
    },
    {
        .type = LOGGER_STREAM_STDERR,
        .handler = {
            logger_stream_standard_create,
            logger_stream_standard_destroy,
            logger_stream_standard_error_flush
        }
    },
    {
        .type = LOGGER_STREAM_FILE,
        .handler = {
            logger_stream_file_create,
            logger_stream_file_destroy,
            logger_stream_file_flush
        }
    },
    {
        .type = LOGGER_STREAM_UDP,
        .handler = {
            logger_stream_socket_create,
            logger_stream_socket_destroy,
            logger_stream_socket_udp_flush
        }
    },
    {
        .type = LOGGER_STREAM_TCP,
        .handler = {
            logger_stream_socket_create,
            logger_stream_socket_destroy,
            logger_stream_socket_tcp_flush
        }
    },
    {
        .type = LOGGER_STREAM_DIRECT,
        .handler = {
            logger_stream_standard_create,
            logger_stream_standard_destroy,
            logger_stream_standard_output_flush
        }
    }
};

const struct logger_stream_handler *logger_stream_config_by_type(
        enum logger_stream_type type) {

    for (size_t i = 0; i < LOGGER_ARRAY_SIZE(gconfig); i++) {
        if (gconfig[i].type == type) {
            return &gconfig[i].handler;
        }
    }

    return NULL;
}
