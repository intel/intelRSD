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

#include "parser_common.hpp"
#include "ssdp/ssdp_packet_parser.hpp"
#include "gtest/gtest.h"

#include <string>

namespace ssdp {

const std::string VALID_M_SEARCH("M-SEARCH * HTTP/1.1\r\n"
        "Host:239.255.255.250:1900\r\n"
        "ST:urn:dmtf-org:service:redfish-rest:1\r\n"
        "MAN:\"ssdp:discover\"\r\n"
        "MX:3\r\n"
        "\r\n");

const std::string VALID_M_SEARCH_RESPONSE("HTTP/1.1 200 OK\r\n"
        "Cache-Control:max-age=1800\r\n"
        "ST:urn:dmtf-org:service:redfish-rest:1\r\n"
        "USN:uuid:7e698e98-f1cc-11e5-bc07-4325b1f9a7b9::urn:dmtf-org:service:redfish-rest:1\r\n"
        "AL:http://192.168.1.1:8888/redfish/v1\r\n"
        "Ext:\r\n"
        "\r\n");

const std::string VALID_NOTIFY("NOTIFY * HTTP/1.1\r\n"
        "Host:239.255.255.250:1900\r\n"
        "Cache-Control:max-age=1800\r\n"
        "NT:urn:dmtf-org:service:redfish-rest:1\r\n"
        "NTS:ssdp:alive\r\n"
        "USN:uuid:7e698e98-f1cc-11e5-bc07-4325b1f9a7b9::urn:dmtf-org:service:redfish-rest:1\r\n"
        "AL:http://192.168.1.1:8888/redfish/v1\r\n"
        "Ext:\r\n"
        "\r\n");

const std::string VALID_BYEBYE_NOTIFY("NOTIFY * HTTP/1.1\r\n"
        "Host:239.255.255.250:1900\r\n"
        "NT:urn:dmtf-org:service:redfish-rest:1\r\n"
        "NTS:ssdp:byebye\r\n"
        "USN:uuid:7e698e98-f1cc-11e5-bc07-4325b1f9a7b9::urn:dmtf-org:service:redfish-rest:1\r\n"
        "\r\n");

PacketParserTest::~PacketParserTest() {}

SsdpPacket PacketParserTest::parse_string(const std::string& s) {
    SsdpPacketParser<std::string> parser(s, s.size());
    return parser.parse();
}

}
