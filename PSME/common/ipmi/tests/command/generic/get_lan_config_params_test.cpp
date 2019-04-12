/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "ipmi/command/generic/get_lan_config_params.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

TEST(GetLanIpAddressRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {1, 3, 0, 0};
    request::GetLanIpAddress request;
    request.set_channel(0x01);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetLanIpAddressResponse, GetIpAddressWithoutDoUnpackCall) {
    std::string expected = "0.0.0.0";
    response::GetLanIpAddress response;
    ASSERT_EQ(expected, response.get_ip_address());
}

TEST(GetLanIpAddressResponse, GetIpAddress) {
    std::string expected = "1.1.1.10";
    ipmi::IpmiInterface::ByteBuffer data = {0x0, 0x11, 0x1, 0x1, 0x1, 0xA};
    response::GetLanIpAddress response;
    response.do_unpack(data);
    ASSERT_EQ(expected, response.get_ip_address());
}

TEST(GetLanIpAddressResponse, ThrowParameterNotSupportedError) {
    ipmi::IpmiInterface::ByteBuffer data = {ipmi::ParameterNotSupportedError::ERROR_CODE};
    response::GetLanIpAddress response;
    ASSERT_THROW(response.do_unpack(data), ipmi::ParameterNotSupportedError);
}

TEST(GetLanIpAddressSourceRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {1, 4, 0, 0};
    request::GetLanIpAddressSource request;
    request.set_channel(0x01);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetLanIpAddressSourceResponse, GetIpAddressWithoutDoUnpackCall) {
    response::GetLanIpAddressSource response;
    ASSERT_EQ(LanConfigIpAddressSource::UNSPECIFIED, response.get_ip_address_source());
}

TEST(GetLanIpAddressSourceResponse, GetIpAddress) {
    ipmi::IpmiInterface::ByteBuffer data = {0x0, 0x11, 0x1};
    response::GetLanIpAddressSource response;
    response.do_unpack(data);
    ASSERT_EQ(LanConfigIpAddressSource::STATIC, response.get_ip_address_source());
}

TEST(GetLanMacAddressRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {1, 5, 0, 0};
    request::GetLanMacAddress request;
    request.set_channel(0x01);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetLanMacAddressResponse, GetIpAddressWithoutDoUnpackCall) {
    std::string expected = "00:00:00:00:00:00";
    response::GetLanMacAddress response;
    ASSERT_EQ(expected, response.get_mac_address());
}

TEST(GetLanMacAddressResponse, GetIpAddress) {
    std::string expected = "aa:bb:cc:dd:ee:ff";
    ipmi::IpmiInterface::ByteBuffer data = {0x0, 0x11, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    response::GetLanMacAddress response;
    response.do_unpack(data);
    ASSERT_EQ(expected, response.get_mac_address());
}
