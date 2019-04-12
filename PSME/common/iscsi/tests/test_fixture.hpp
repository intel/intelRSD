/*!
 * @brief Test fixture of the iSCSI MDR parser.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include "iscsi/iscsi_mdr_parser.hpp"

#include <vector>
#include <string>

extern "C" {
#include <arpa/inet.h>
}

namespace fixture {

std::vector<uint8_t> compose_iscsi_mdr_blob(int initiators, int targets, int attempts);

static const int MAC_ADDRESS_SIZE = 6;
static const int IP_ADDRESS_SIZE = 16;
static const char* IPv4_ADDRESS_1 = "192.168.2.33";
static const char* IPv4_MASK = "255.255.255.0";
static const char* IPv4_ADDRESS_2 = "192.168.2.34";
static const char* IPv6_ADDRESS = "2001:db8:8714:3a90::12";
static const uint8_t MAC_ADDRESS[MAC_ADDRESS_SIZE] = { 0x02, 0x42, 0xb9, 0x23, 0x01, 0x02 };

static uint16_t HANDLE = 0;

static const std::string INITIATOR_NAME = "Some initiator name";
static const std::string TARGET_NAME = "Some target name";
static const std::string CHAP_USER_NAME = "Some CHAP name";
static const std::string CHAP_USER_SECRET = "Some CHAP ******";
static const std::string CHAP_MUTUAL_NAME = "Some CHAP mutual name";
static const std::string CHAP_MUTUAL_SECRET = "Some CHAP mutual ******";

template<typename T>
void push_back_multibyte(std::vector<uint8_t>& blob, T val) {
    for (size_t i = 0; i < sizeof(val); ++i) {
        blob.push_back(uint8_t(val & 0xFF));
        val = static_cast<T>(val >> 8);
    }
}

template<>
void push_back_multibyte(std::vector<uint8_t>& blob, in_addr val) {
    push_back_multibyte(blob, val.s_addr);
    // if an IPv4 address is sent, fill rest of field with zeros
    for (size_t i = 0; i < IP_ADDRESS_SIZE - sizeof(val); ++i) {
        blob.push_back(0);
    }
}

template<>
void push_back_multibyte(std::vector<uint8_t>& blob, in6_addr val) {
    blob.insert(blob.end(), val.s6_addr, val.s6_addr + IP_ADDRESS_SIZE);
}

static void add_version(std::vector<uint8_t>& blob, uint8_t major, uint8_t minor) {
    static uint16_t length = 7; // the size of the version structure
    blob.push_back(0x01); // parameter id
    push_back_multibyte(blob, length); // length
    push_back_multibyte(blob, HANDLE++); // handle

    blob.push_back(major); // Major version
    blob.push_back(minor); // Minor version
    // ending double zeros
    blob.push_back(0x00);
    blob.push_back(0x00);
}

static void add_initiator(std::vector<uint8_t>& blob) {
    static uint16_t length = 94; // the size of the initiator structure
    blob.push_back(0x02); // parameter id
    push_back_multibyte(blob, length); // length
    push_back_multibyte(blob, HANDLE++); // handle

    blob.push_back(0x00); // No DHCP
    blob.push_back(0x01); // first string ref
    blob.push_back(0x01); // IPv4
    in6_addr tmp_6_addr;
    in_addr tmp_4_addr;
    inet_pton(AF_INET, IPv4_ADDRESS_1, &tmp_4_addr);
    push_back_multibyte(blob, tmp_4_addr); // initiator IP
    inet_pton(AF_INET, IPv4_MASK, &tmp_4_addr);
    push_back_multibyte(blob, tmp_4_addr); // mask
    inet_pton(AF_INET, IPv4_ADDRESS_2, &tmp_4_addr);
    push_back_multibyte(blob, tmp_4_addr); // gateway
    inet_pton(AF_INET6, IPv6_ADDRESS, &tmp_6_addr);
    push_back_multibyte(blob, tmp_6_addr); // prim DNS
    inet_pton(AF_INET, IPv4_ADDRESS_2, &tmp_4_addr);
    push_back_multibyte(blob, tmp_4_addr); // sec DNS
    blob.insert(blob.end(), MAC_ADDRESS, MAC_ADDRESS + MAC_ADDRESS_SIZE); // MAC address

    // Initiator name
    blob.insert(blob.end(), INITIATOR_NAME.cbegin(), INITIATOR_NAME.cend());
    blob.push_back(0x00);

    // ending zero
    blob.push_back(0x00);
}

static void add_target(std::vector<uint8_t>& blob) {
    static uint16_t length = 37; // the size of the target structure
    blob.push_back(0x03); // parameter id
    push_back_multibyte(blob, length); // length
    push_back_multibyte(blob, HANDLE++); // handle

    blob.push_back(0x01); // DHCP on
    blob.push_back(0x01); // Target name
    blob.push_back(0x02); // IPv6
    in_addr tmp_4_addr;
    inet_pton(AF_INET, IPv4_ADDRESS_1, &tmp_4_addr);
    push_back_multibyte(blob, tmp_4_addr); // target IP
    uint16_t port = 0x1F90;
    push_back_multibyte(blob, port); // port 8080
    uint16_t lun = 0x64;
    push_back_multibyte(blob, lun); // LUN 100
    blob.push_back(0x01); // VLAN enabled
    uint16_t vlan_id = 0x0102;
    push_back_multibyte(blob, vlan_id); // VLAN ID
    blob.push_back(0x00); // no router advertisement
    blob.push_back(0x02); // mutual CHAP
    blob.push_back(0x02); // CHAP user name
    blob.push_back(0x03); // CHAP secret
    blob.push_back(0x04); // mutual CHAP name
    blob.push_back(0x05); // mutual CHAP secret

    // Target name
    blob.insert(blob.end(), TARGET_NAME.cbegin(), TARGET_NAME.cend());
    blob.push_back(0x00);
    // CHAP user name
    blob.insert(blob.end(), CHAP_USER_NAME.cbegin(), CHAP_USER_NAME.cend());
    blob.push_back(0x00);
    // CHAP secret
    blob.insert(blob.end(), CHAP_USER_SECRET.cbegin(), CHAP_USER_SECRET.cend());
    blob.push_back(0x00);
    // mutual CHAP name
    blob.insert(blob.end(), CHAP_MUTUAL_NAME.cbegin(), CHAP_MUTUAL_NAME.cend());
    blob.push_back(0x00);
    // mutual CHAP secret
    blob.insert(blob.end(), CHAP_MUTUAL_SECRET.cbegin(), CHAP_MUTUAL_SECRET.cend());
    blob.push_back(0x00);

    // ending zero
    blob.push_back(0x00);
}

static void add_attempt(std::vector<uint8_t>& blob) {
    static uint16_t length = 12; // the size of the target structure
    blob.push_back(0x04); // parameter id
    push_back_multibyte(blob, length); // length
    push_back_multibyte(blob, HANDLE++); // handle

    // Initiator handle
    uint16_t init_handle = 0x02;
    push_back_multibyte(blob, init_handle);
    // Target handle
    uint16_t tgt_handle = 0x04;
    push_back_multibyte(blob, tgt_handle);
    // Connection wait time
    uint16_t wait_time = 0x0400;
    push_back_multibyte(blob, wait_time);
    // Connection retry count
    blob.push_back(0x10);

    // ending double zeros
    blob.push_back(0x00);
    blob.push_back(0x00);
}

std::vector<uint8_t> compose_iscsi_mdr_blob(int initiators, int targets, int attempts) {
    std::vector<uint8_t> vec;

    add_version(vec, 0x01, 0x00);

    for (int i = 0; i < initiators; i++) {
        add_initiator(vec);
    }

    for (int i = 0; i < targets; i++) {
        add_target(vec);
    }

    for (int i = 0; i < attempts; i++) {
        add_attempt(vec);
    }

    return vec;
}

} // fixture
