/*!
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
 *
 * @file br_score_message_header.cpp
 *
 * @brief LLDP Bridge score message header
 *
 *  +------------------------------------+
 *  |    BRIDGE SCORE MESSAGE HEADER     |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | Bridge score    | 2     | 00       |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/br_score_message_header.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;

BrScoreMessageHeader::BrScoreMessageHeader(Score score) : m_score{score} {
}

void BrScoreMessageHeader::parse_raw(const string&) { }

std::string BrScoreMessageHeader::get_raw() const {
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0') << std::hex;
    raw_msg << std::setw(2) << uint16_t(m_score);
    return raw_msg.str();
}

size_t BrScoreMessageHeader::get_raw_size() const {
    return (sizeof(m_score) * 2);
}

BrScoreMessageHeader::~BrScoreMessageHeader() {
}
