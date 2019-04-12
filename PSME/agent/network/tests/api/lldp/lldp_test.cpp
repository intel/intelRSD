/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */


#include "gtest/gtest.h"

#include "api/lldp/arg_message_header.hpp"
#include "api/lldp/arg_value_message_header.hpp"
#include "api/lldp/br_score_message_header.hpp"
#include "api/lldp/command_message_header.hpp"
#include "api/lldp/tlv_message_header.hpp"
#include "api/lldp/tlv_id_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/tlv_id.hpp"
#include "api/lldp/response.hpp"
#include "api/lldp/request.hpp"
#include "api/lldp/client.hpp"
#include "api/lldp/port_desc_tlv.hpp"

#include "mock_clif.hpp"

#include <string>

using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;
using namespace std;

class LLDPTest: public ::testing::Test {
protected:
public:
    virtual ~LLDPTest();
};

LLDPTest::~LLDPTest() {}

TEST_F(LLDPTest, arg_message_header_test) {
    const string input_arg{"SomeArgument"};
    const ArgMessageHeader amh{input_arg};
    const string raw = amh.get_raw();
    unsigned short len{};
    sscanf(raw.substr(0, 2).c_str(), "%02hX", &len);
    ASSERT_EQ(len, input_arg.length());
    ASSERT_EQ(input_arg, raw.substr(2, len));
    ASSERT_EQ(amh.get_raw_size(), input_arg.length() + 2);
}

TEST_F(LLDPTest, arg_value_message_header_get_raw_test) {
    const string input_name{"SomeName"};
    const string input_value{"SomeValue"};
    const ArgValueMessageHeader amh{input_name, input_value};
    const string raw = amh.get_raw();
    unsigned short name_len{};
    unsigned value_len{};
    sscanf(raw.substr(0, 2).c_str(), "%02hX", &name_len);
    sscanf(raw.substr(2 + name_len, 4).c_str(), "%04X", &value_len);
    ASSERT_EQ(name_len, input_name.length());
    ASSERT_EQ(input_name, raw.substr(2, name_len));
    ASSERT_EQ(value_len, input_value.length());
    ASSERT_EQ(input_value, raw.substr(6 + name_len, value_len));
    ASSERT_EQ(amh.get_raw_size(), name_len + value_len + 6);
}

TEST_F(LLDPTest, br_score_message_header_test) {
    const BrScoreMessageHeader::Score input_arg{};
    const BrScoreMessageHeader mh{input_arg};
    const string raw = mh.get_raw();
    ASSERT_EQ(raw.length(), 2);
    unsigned short score{};
    sscanf(raw.substr(0, 2).c_str(), "%02hX", &score);
    ASSERT_EQ(score, uint16_t(input_arg));
    ASSERT_EQ(mh.get_raw_size(), raw.length());
}

TEST_F(LLDPTest, command_message_header_test_pass) {
    const CommandMessageHeader::CmdType input_type{CommandMessageHeader::CmdType::RESPONSE};
    const CommandMessageHeader::CmdCode input_code{};
    const string ifname{"sw0p1"};
    const uint32_t input_option{};
    const CommandMessageHeader mh{ifname, input_type, input_code, input_option};
    const string raw = mh.get_raw();
    ASSERT_EQ(raw[0], 'R');
    ASSERT_EQ(raw[1], '3');
    unsigned short code{};
    sscanf(raw.substr(2, 2).c_str(), "%02hX", &code);
    ASSERT_EQ(code, uint16_t(input_code));
    uint64_t option{};
    sscanf(raw.substr(4, 8).c_str(), "%08lX", &option);
    ASSERT_EQ(option, uint64_t(input_option));
    unsigned short len{};
    sscanf(raw.substr(12, 2).c_str(), "%02hX", &len);
    ASSERT_EQ(len, uint16_t(ifname.length()));
    ASSERT_EQ(ifname, raw.substr(14, len));
    ASSERT_EQ(mh.get_raw_size(), raw.length());
}

TEST_F(LLDPTest, command_message_header_test_get_raw_unknown_cmd) {
    const CommandMessageHeader::CmdType input_type{CommandMessageHeader::CmdType::UNKNOWN};
    const CommandMessageHeader::CmdCode input_code{};
    const string ifname{"sw0p1"};
    const uint32_t input_option{};
    const CommandMessageHeader mh{ifname, input_type, input_code, input_option};
    ASSERT_THROW(mh.get_raw(), ApiError);
}

TEST_F(LLDPTest, command_message_header_test_parse_raw_pass) {
    CommandMessageHeader mh{};
    char raw[] = "R3010000000105sw0p1";
    ASSERT_NO_THROW(mh.parse_raw(raw));
}

TEST_F(LLDPTest, command_message_header_test_parse_raw_too_short) {
    CommandMessageHeader mh{};
    char raw[] = "R301";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, command_message_header_test_parse_raw_unknown_cmd) {
    CommandMessageHeader mh{};
    char raw[] = "x3010000000105sw0p1";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, command_message_header_test_parse_raw_unknown_version) {
    CommandMessageHeader mh{};
    char raw[] = "R1010000000105sw0p1";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, command_message_header_test_parse_raw_too_short_ifname) {
    CommandMessageHeader mh{};
    char raw[] = "R3010000000105";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, tlv_message_header_test_parse_raw_pass) {
    TlvMessageHeader mh{};
    char raw[] = "000412345678";
    ASSERT_NO_THROW(mh.parse_raw(raw));
}

TEST_F(LLDPTest, tlv_message_header_test_parse_raw_header_too_short) {
    TlvMessageHeader mh{};
    char raw[] = "00";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, tlv_message_header_test_parse_raw_too_short) {
    TlvMessageHeader mh{};
    char raw[] = "00041234";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, tlv_id_message_header_test_parse_raw_pass) {
    TlvIdMessageHeader mh{};
    char raw[] = "00000000";
    ASSERT_NO_THROW(mh.parse_raw(raw));
}

TEST_F(LLDPTest, tlv_id_message_header_test_parse_raw_too_short) {
    TlvIdMessageHeader mh{};
    char raw[] = "000000";
    ASSERT_THROW(mh.parse_raw(raw), ApiError);
}

TEST_F(LLDPTest, tlv_id_message_header_test_get_raw) {
    TlvId id{TlvId::PORT_DESCRIPTION};
    TlvIdMessageHeader mh{id};
    string raw = mh.get_raw();
    ASSERT_EQ("00000004", raw);
}

TEST_F(LLDPTest, tlv_payload_resize) {
    TlvPayload p{};
    ASSERT_NO_THROW(p.resize(128));
}

TEST_F(LLDPTest, response_pass) {
    static constexpr char raw[] = "R00";
    ASSERT_NO_THROW(Response{raw});
}

TEST_F(LLDPTest, response_too_short) {
    static constexpr char raw[] = "R0";
    ASSERT_THROW(Response{raw}, ::ApiError);
}

TEST_F(LLDPTest, response_invalid_type) {
    static constexpr char raw[] = "x00";
    ASSERT_THROW(Response{raw}, ApiError);
}

TEST_F(LLDPTest, response_cmd_success) {
    static constexpr char raw[] = "R00";
    Response resp{raw};
    ASSERT_EQ(true, bool(resp));
}

TEST_F(LLDPTest, response_pop_header) {
    static constexpr char raw[] = "R00R3010000000105sw0p1";
    Response resp{raw};
    CommandMessageHeader header{};
    ASSERT_NO_THROW(header = resp.pop<CommandMessageHeader>());
}

TEST_F(LLDPTest, response_pop_header_eof) {
    static constexpr char raw[] = "R00";
    Response resp{raw};
    CommandMessageHeader header{};
    ASSERT_THROW(header = resp.pop<CommandMessageHeader>(), ApiError);
}

TEST_F(LLDPTest, response_get_status_success) {
    static constexpr char raw[] = "R00";
    Response resp{raw};
    ASSERT_EQ(0, resp.get_status());
}

TEST_F(LLDPTest, response_get_status_failure) {
    static constexpr char raw[] = "R99";
    Response resp{raw};
    ASSERT_NE(0, resp.get_status());
}

TEST_F(LLDPTest, request_get_message) {
    Request req{};
    ASSERT_NO_THROW(req.get_message());
}

/*
TEST_F(LLDPTest, client_get_neighbor_tlv_pass) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3010000000205sw0p1000000040000");
    ASSERT_NO_THROW(c.get_neighbor_tlv<PortDescTlv>());
}

TEST_F(LLDPTest, client_get_neighbor_tlv_no_port_desc) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3010000000205sw0p100000004");
    ASSERT_THROW(c.get_neighbor_tlv<PortDescTlv>(), ApiError);
}

TEST_F(LLDPTest, client_get_neighbor_tlv_wrong_tlv_id) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3010000000205sw0p100000003");
    ASSERT_THROW(c.get_neighbor_tlv<PortDescTlv>(), ApiError);
}

TEST_F(LLDPTest, client_get_neighbor_tlv_command_header_error) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3010000000205sw0p10000000");
    ASSERT_THROW(c.get_neighbor_tlv<PortDescTlv>(), ApiError);
}

TEST_F(LLDPTest, client_get_neighbor_tlv_no_conn) {
    Client c{"sw0p1"};
    mock_expect_no_conn();
    ASSERT_THROW(c.get_neighbor_tlv<PortDescTlv>(), ApiError);
}

TEST_F(LLDPTest, client_get_neighbor_tlv_no_agent) {
    Client c{"sw0p1"};
    mock_expect_reply("R03");
    ASSERT_THROW(c.get_neighbor_tlv<PortDescTlv>(), ApiError);
}

TEST_F(LLDPTest, client_enable_tlv_pass) {
    Client c{"sw0p1"};
    TlvId id{TlvId::PORT_DESCRIPTION};
    mock_expect_reply("R00R3020000000C05sw0p1000000040000");
    ASSERT_NO_THROW(c.enable_tlv(id, true));
}

TEST_F(LLDPTest, client_enable_tlv_wrong_id) {
    Client c{"sw0p1"};
    TlvId id{TlvId::PORT_DESCRIPTION};
    mock_expect_reply("R00R3020000000C05sw0p1000000030000");
    ASSERT_THROW(c.enable_tlv(id, true), ApiError);
}

TEST_F(LLDPTest, client_enable_tlv_wrong_code) {
    Client c{"sw0p1"};
    TlvId id{TlvId::PORT_DESCRIPTION};
    mock_expect_reply("R00R3020000000105sw0p1000000030000");
    ASSERT_THROW(c.enable_tlv(id, true), ApiError);
}

TEST_F(LLDPTest, client_set_mode_pass) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3040000000C05sw0p1000000040000");
    ASSERT_NO_THROW(c.set_mode(Client::Mode::RX));
}

TEST_F(LLDPTest, client_set_mode_invalid_code) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3060000000C05sw0p1000000040000");
    ASSERT_THROW(c.set_mode(Client::Mode::RX), ApiError);
}


TEST_F(LLDPTest, client_get_mode_pass) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3030000000405sw0p10BadminStatus0002rx");
    ASSERT_NO_THROW(c.get_mode());
}

TEST_F(LLDPTest, client_get_mode_invalid_admin) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3030000000405sw0p10BadminStatus0002xx");
    ASSERT_THROW(c.get_mode(), ApiError);
}

TEST_F(LLDPTest, client_get_mode_invalid_response) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3030000000405sw0p10BrdminStatus0002xx");
    ASSERT_THROW(c.get_mode(), ApiError);
}

TEST_F(LLDPTest, client_get_mode_invalid_code) {
    Client c{"sw0p1"};
    mock_expect_reply("R00R3050000000405sw0p10BrdminStatus0002xx");
    ASSERT_THROW(c.get_mode(), ApiError);
}
*/
