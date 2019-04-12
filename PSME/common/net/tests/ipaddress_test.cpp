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
 * */

#include <cstdio>
#include <fstream>
#include <sstream>

#include "net/ipaddress.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace net;

namespace {

const std::string IPv4_ANY_ADDR_SRTRING = "0.0.0.0";
const std::string IPv4_LOOPBACK_ADDR_STRING = "127.0.0.1";

}

TEST(IpAddressTest, DefaultConstructor) {
  IpAddress addr;
  EXPECT_TRUE(addr.is_any_address());
  EXPECT_EQ(sizeof(in_addr), addr.length());
  EXPECT_EQ(AddressFamily::IPv4, addr.get_address_family());
  EXPECT_EQ(AF_INET, addr.get_native_address_family());
  EXPECT_EQ(IPv4_ANY_ADDR_SRTRING, addr.to_string());
}

TEST(IpAddressTest, IPv4Constructor) {
  in_addr ipv4;
  ipv4.s_addr = 0;

  IpAddress addr(ipv4);
  EXPECT_TRUE(addr.is_any_address());
  EXPECT_EQ(sizeof(in_addr), addr.length());
  EXPECT_EQ(AddressFamily::IPv4, addr.get_address_family());
  EXPECT_EQ(AF_INET, addr.get_native_address_family());
  EXPECT_EQ(IPv4_ANY_ADDR_SRTRING, addr.to_string());
}

TEST(IpAddressTest, FromString) {
  IpAddress actual = IpAddress::from_string(IPv4_ANY_ADDR_SRTRING);

  EXPECT_EQ(IPv4_ANY_ADDR_SRTRING, actual.to_string());

  actual = IpAddress::from_string(IPv4_LOOPBACK_ADDR_STRING);

  EXPECT_EQ(IPv4_LOOPBACK_ADDR_STRING, actual.to_string());
}
