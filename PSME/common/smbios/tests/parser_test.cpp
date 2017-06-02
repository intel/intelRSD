/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "test_fixture.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::Return;
using ::testing::Throw;
using namespace smbios::parser;

class SmbiosParserTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        m_entry_point = fixture::entry_point.entry_point;
    }

    virtual void TearDown() { }
    virtual ~SmbiosParserTest();

    union {
        SmbiosEntryPoint m_entry_point;
        uint8_t m_blob[1024];
    };

    uint8_t* get_blob() {
       return reinterpret_cast<uint8_t*>(&m_blob);
    }
};

SmbiosParserTest::~SmbiosParserTest(){}


TEST_F(SmbiosParserTest, ExceptionWhenBlobIsNull) {
    ASSERT_THROW(SmbiosParser(nullptr, 100000), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, ExceptionWhenBlobIsTooShort) {
    ASSERT_THROW(SmbiosParser(m_blob, 10), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, AnchorStringIncorrect) {
    m_entry_point.anchor[0] = 'A'; // invalid anchor string
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, IntermediateAnchorStringIncorrect) {
    m_entry_point.intermediate_anchor[0] = 'A'; // invalid anchor string
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, ChecksumIncorrect) {
    m_entry_point.checksum = 123; // invalid checksum
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, IntermediateChecksumIncorrect) {
    m_entry_point.intermediate_checksum = 123; // invalid checksum
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosParser::Exception);
}

TEST_F(SmbiosParserTest, AcceptsCorrectEntryPoint) {
    ASSERT_NO_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)));
}

TEST_F(SmbiosParserTest, ReadAndParseCompleteBlob) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}

TEST_F(SmbiosParserTest, IterateOverSystems) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    auto systems = parser.get_all<SMBIOS_SYSTEM_INFO_DATA>();
    ASSERT_EQ(1, systems.size());
    EXPECT_STREQ("ABCDEF", systems[0].get_string(systems[0].data.manufacturer).c_str());
}

TEST_F(SmbiosParserTest, IterateOverPcie) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    auto pcie = parser.get_all<SMBIOS_PCIE_INFO_DATA>();
    ASSERT_EQ(3, pcie.size());
    EXPECT_EQ(0xee, pcie[0].data.SlotNo);
    EXPECT_EQ(0x04, pcie[1].data.SlotNo);
    EXPECT_EQ(0x00, pcie[2].data.SlotNo);
}

TEST_F(SmbiosParserTest, IterateOverNics) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    auto nic = parser.get_all<SMBIOS_NIC_INFO_DATA>();
    ASSERT_EQ(2, nic.size());
    EXPECT_EQ(0x1234, nic[0].data.VendorID);
    EXPECT_EQ(0x2244, nic[1].data.VendorID);
}

TEST_F(SmbiosParserTest, IterateOverStorage) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    auto hdd = parser.get_all<SMBIOS_STORAGE_INFO_DATA>();//(nullptr, SMBIOS_STORAGE_INFO_DATA::LEN);
    ASSERT_EQ(1, hdd.size());
    EXPECT_STREQ("IMPEL SSD", reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDName));
}

TEST_F(SmbiosParserTest, IterateOverStorageV2) {
    SmbiosParser parser(reinterpret_cast<const uint8_t *>(fixture::smbios), sizeof(fixture::smbios));

    auto hdd = parser.get_all<SMBIOS_STORAGE_INFO_DATA_V2>();//(nullptr, SMBIOS_STORAGE_INFO_DATA_V2::LEN);
    ASSERT_EQ(1, hdd.size());
    EXPECT_STREQ("IMPEL SSD                               ",
                 std::string(reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDName),
                                          HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE).c_str());
    EXPECT_STREQ("VCIL134410FX21E0    ",
                 std::string(reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDSerialNumber),
                                          HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE).c_str());
}
