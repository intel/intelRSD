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
 * @file logger_assert.h
 *
 * @brief Logger assert interface
 * */

#ifndef LOGGER_ASSERT_H
#define LOGGER_ASSERT_H

#include <assert.h>

#include "logger/logger.h"

/*!
 * @def logger_assert(_cond)
 * Assert condition
 * */
#define logger_assert(_cond)        assert(_cond)

#endif /* LOGGER_ASSERT_H */
