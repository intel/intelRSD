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
 **/

#include "ssdp/ssdp_packet_parser.hpp"

namespace ssdp {

const char HTTP_VERSION[] = "HTTP/1.1";
const char CRLF[] = "\r\n";

std::string& trim_right(std::string& str) {
    auto pos = int(str.size()) - 1;
    while (pos >= 0 && std::isspace(str[pos])) {
        --pos;
    }
    str.resize(pos + 1);
    return str;
}

PacketParserException::PacketParserException(const std::string& msg)
    : std::runtime_error("SsdpPacketParser - " + msg) {}

PacketParserException::~PacketParserException() {}

}
