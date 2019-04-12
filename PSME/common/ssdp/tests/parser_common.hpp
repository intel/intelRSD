/*!
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
 * */

#pragma once

#include "gtest/gtest.h"

namespace ssdp {

class SsdpPacket;

extern const std::string VALID_M_SEARCH;
extern const std::string VALID_M_SEARCH_RESPONSE;
extern const std::string VALID_NOTIFY;
extern const std::string VALID_BYEBYE_NOTIFY;

class PacketParserTest : public ::testing::Test {
protected:
    ~PacketParserTest() override;
    SsdpPacket parse_string(const std::string& s);
};

}
