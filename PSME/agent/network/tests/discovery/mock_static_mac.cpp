/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */

#include "hal/static_mac.hpp"

using namespace agent::network::hal;

void StaticMac::get_macs (const std::string&, StaticMac::Table& macs) {
    macs.insert(std::make_pair("A1:12:23:34:45:45", 4000));
    macs.insert(std::make_pair("A2:12:23:34:45:45", 3000));
}
