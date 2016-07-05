/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file clear_boot_valid_bit.cpp
 *
 * @brief Clear Boot Valid Bit command implementation.
 * */

#include "ipmi/command/generic/clear_boot_valid_bit.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::ClearBootValidBit::ClearBootValidBit():
    request::SetSystemBootOptions() {}

request::ClearBootValidBit::~ClearBootValidBit() {}

void request::ClearBootValidBit::pack(vector<uint8_t>& data) const {
    (void)data;
    // @TODO:
    // 1. Create const attributes for ClearBootValidBit.
    // 2. Pack attributes to vector.
}

response::ClearBootValidBit::ClearBootValidBit():
    response::SetSystemBootOptions(){}

response::ClearBootValidBit::~ClearBootValidBit() {}

void response::ClearBootValidBit::unpack(const vector<uint8_t>& data) {
    if(!is_response_correct(data)) {
        return;
    }
}
