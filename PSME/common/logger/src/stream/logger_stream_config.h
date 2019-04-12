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
 * @file logger_stream_config.h
 *
 * @brief Logger stream configuration interface
 * */

#ifndef LOGGER_STREAM_CONFIG_H
#define LOGGER_STREAM_CONFIG_H

#include "logger/logger.h"
#include "logger/stream.h"
#include "logger_stream_instance.h"

/*!
 * @brief Get logger stream handler for given logger stream type
 *
 * @param[in]   type    Logger stream type
 * @return      When success return logger stream handlers object
 *              otherwise return NULL
 * */
const struct logger_stream_handler *logger_stream_config_by_type(
        enum logger_stream_type type);

#endif /* LOGGER_STREAM_CONFIG_H */
