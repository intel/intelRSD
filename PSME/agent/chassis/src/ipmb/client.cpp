/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 * @file client.cpp
 * @brief Perform client I2C (outbound get/inbound response) with MUX
*/

#include "ipmb/client.hpp"

extern "C" {
#include <fcntl.h>
}

using namespace agent::chassis::ipmb;

Client::Client() : Mux() {
    connect();
    set_nonblocking(true);  // Perform all packet communications in non-blocking mode
}

byte_vec_t Client::mode() const {
    return {'C'};
}
