/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file tests/mbr_partition_table.cpp
 */

#include "partition/gpt_partition_table.hpp"
#include "partition/mbr_partition_table.hpp"
#include "raw_device_data.hpp"
#include "mock_drive_interface.hpp"
#include "crc/crc32.hpp"

#include "gtest/gtest.h"

#include <algorithm>

using namespace partition;
using namespace utils;
using namespace crc;

namespace {

/* Used to test protected GptPartitionTable methods */
class GetProtectedGptMembers final : public GptPartitionTable {
public:
    virtual ~GetProtectedGptMembers() {}

    using GptTableData = GptPartitionTable::GptTableData;
    using GptPartitionData = GptPartitionTable::GptPartitionData;
    using GptHeaderData = GptPartitionTable::GptHeaderData;

    static GptHeaderData do_bb_to_gpt_header(const ByteBuffer& bb) {
        return GptPartitionTable::bb_to_gpt_header(bb);
    }

    static GptTableData do_bb_to_gpt_table(const ByteBuffer& bb) {
        return GptPartitionTable::bb_to_gpt_table(bb);
    }

    static ByteBuffer do_gpt_header_to_bb(const GptHeaderData& gpt) {
        return GptPartitionTable::gpt_header_to_bb(gpt);
    }

    static ByteBuffer do_gpt_table_to_bb(const GptTableData& gpt) {
        return GptPartitionTable::gpt_table_to_bb(gpt);
    }

    static std::string do_get_guid(const ByteBuffer& bb, uint64_t offset) {
        return GptPartitionTable::get_guid(bb, offset);
    }

    static std::string do_get_name(const ByteBuffer& bb, uint64_t offset) {
        return GptPartitionTable::get_name(bb, offset);
    }

    static void do_set_guid(ByteBuffer& bb, uint64_t offset, const std::string& guid) {
        GptPartitionTable::set_guid(bb, offset, guid);
    }

    static void do_set_name(ByteBuffer& bb, uint64_t offset, const std::string& name) {
        GptPartitionTable::set_name(bb, offset, name);
    }

};

// constants
static constexpr uint64_t EXPECTED_GPT_SIGNATURE = 0x5452415020494645;
static constexpr uint32_t EXPECTED_GPT_REVISION = 0x00010000;
static constexpr uint32_t EXPECTED_TEST_GPT_HEADER_CRC = 0x9D3C39D8;
static constexpr uint32_t EXPECTED_TEST_GPT_TABLE_CRC = 0xAB54D286;
static constexpr uint32_t EXPECTED_GPT_PARTITION_COUNT = 128;
static constexpr char EXPECTED_LINUX_PARTITION_GUID[] = "0FC63DAF-8483-4772-8E79-3D69D8477DE4";

// sizes in bytes
static constexpr uint64_t LBA_SIZE = 512;
static constexpr uint64_t EXPECTED_MBR_SIZE = 512;
static constexpr uint64_t EXPECTED_GPT_TABLE_SIZE = 16384;
static constexpr uint64_t EXPECTED_GPT_HEADER_SIZE = 92;
static constexpr uint32_t EXPECTED_GPT_PARTITION_SIZE = 128;

// sizes/locations in LBA (test values)
static constexpr uint64_t TEST_DRIVE_SIZE_LBA = 100;
static constexpr uint64_t TEST_TABLE_SIZE_LBA = EXPECTED_GPT_TABLE_SIZE / LBA_SIZE;
static constexpr uint64_t TEST_PRIMARY_HEADER_LBA = 1;
static constexpr uint64_t TEST_PRIMARY_TABLE_LBA = 2;
static constexpr uint64_t TEST_FIRST_USABLE_LBA = TEST_PRIMARY_TABLE_LBA + TEST_TABLE_SIZE_LBA + 1;
static constexpr uint64_t TEST_BACKUP_HEADER_LBA = TEST_DRIVE_SIZE_LBA - 1;
static constexpr uint64_t TEST_BACKUP_TABLE_LBA = TEST_BACKUP_HEADER_LBA - TEST_TABLE_SIZE_LBA;
static constexpr uint64_t TEST_LAST_USABLE_LBA = TEST_BACKUP_TABLE_LBA - 1;

void make_mbr(std::shared_ptr<MockDriveInterface> mdi) {
    PartitionData pd{};
    pd.type = PartitionType::MBR_PROTECTIVE;
    MbrPartitionTable mbr{};
    mbr.reset(100);
    mbr.set_partition(0, pd);
    mbr.update();
    mbr.save({}, mdi);
}

void make_gpt(std::shared_ptr<MockDriveInterface> mdi) {

    ASSERT_TRUE(TEST_LAST_USABLE_LBA >= TEST_FIRST_USABLE_LBA); // cross check if this is not too short

    mdi->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);
    // zero fields (if there was anything else before)
    std::fill(mdi->mocked_data.get_raw_vector().begin(),
              mdi->mocked_data.get_raw_vector().begin() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_TABLE_SIZE,
              0);
    make_mbr(mdi);

    // fill gpt header
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 0, EXPECTED_GPT_SIGNATURE);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 8, EXPECTED_GPT_REVISION);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 12, EXPECTED_GPT_HEADER_SIZE);
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 24, TEST_PRIMARY_HEADER_LBA);
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 32, TEST_BACKUP_HEADER_LBA);
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 40, TEST_FIRST_USABLE_LBA);
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 48, TEST_LAST_USABLE_LBA);
    mdi->mocked_data.set_le64(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 72, TEST_PRIMARY_TABLE_LBA);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 80, EXPECTED_GPT_PARTITION_COUNT);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 84, EXPECTED_GPT_PARTITION_SIZE);

    // set CRCs with expected values
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, EXPECTED_TEST_GPT_HEADER_CRC);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, EXPECTED_TEST_GPT_TABLE_CRC);
}

void update_gpt_backup(std::shared_ptr<MockDriveInterface> mdi) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    // copy primary header to the backup location
    std::copy(mdi->mocked_data.get_raw_vector().begin() + TEST_PRIMARY_HEADER_LBA * LBA_SIZE,
              mdi->mocked_data.get_raw_vector().begin() + TEST_PRIMARY_HEADER_LBA * LBA_SIZE + EXPECTED_GPT_HEADER_SIZE,
              mdi->mocked_data.get_raw_vector().begin() + TEST_BACKUP_HEADER_LBA * LBA_SIZE);
    // copy primary table to the backup location
    std::copy(mdi->mocked_data.get_raw_vector().begin() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE,
              mdi->mocked_data.get_raw_vector().begin() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_TABLE_SIZE,
              mdi->mocked_data.get_raw_vector().begin() + TEST_BACKUP_TABLE_LBA * LBA_SIZE);
    mdi->mocked_data.set_le64(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 24, TEST_BACKUP_HEADER_LBA);
    mdi->mocked_data.set_le64(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 32, TEST_PRIMARY_HEADER_LBA);
    mdi->mocked_data.set_le64(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 72, TEST_LAST_USABLE_LBA + 1);
}

void update_backup_crcs(std::shared_ptr<MockDriveInterface> mdi) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    mdi->mocked_data.set_le32(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 16, 0);
    Crc32 table_crc(mdi->mocked_data.data() + (TEST_LAST_USABLE_LBA + 1) * LBA_SIZE, EXPECTED_GPT_TABLE_SIZE);
    mdi->mocked_data.set_le32(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 88, table_crc);
    Crc32 header_crc(mdi->mocked_data.data() + TEST_BACKUP_HEADER_LBA * LBA_SIZE, EXPECTED_GPT_HEADER_SIZE);
    mdi->mocked_data.set_le32(TEST_BACKUP_HEADER_LBA * LBA_SIZE + 16, header_crc);
}

void update_crcs(std::shared_ptr<MockDriveInterface> mdi) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0);
    Crc32 table_crc(mdi->mocked_data.data() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE, EXPECTED_GPT_TABLE_SIZE);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, table_crc);
    Crc32 header_crc(mdi->mocked_data.data() + TEST_PRIMARY_HEADER_LBA * LBA_SIZE, EXPECTED_GPT_HEADER_SIZE);
    mdi->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, header_crc);
}

void add_partition(std::shared_ptr<MockDriveInterface> mdi, unsigned slot, uint64_t first, uint64_t last) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    uint64_t offset = TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_PARTITION_SIZE * slot;
    GetProtectedGptMembers::do_set_guid(mdi->mocked_data, offset, EXPECTED_LINUX_PARTITION_GUID);
    mdi->mocked_data.set_le64(offset + 32, first);
    mdi->mocked_data.set_le64(offset + 40, last);
}

void add_partition(std::shared_ptr<MockDriveInterface> mdi, unsigned slot) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    GetProtectedGptMembers::do_set_guid(mdi->mocked_data,
        TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_PARTITION_SIZE * slot,
        EXPECTED_LINUX_PARTITION_GUID);
}

void add_partition(std::shared_ptr<MockDriveInterface> mdi, unsigned slot, const std::string& name,
        const std::string& type, uint64_t start, uint64_t end) {
    ASSERT_EQ(TEST_DRIVE_SIZE_LBA * LBA_SIZE, mdi->mocked_data.size());
    uint64_t offset = TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_PARTITION_SIZE * slot;
    GetProtectedGptMembers::do_set_guid(mdi->mocked_data, offset, type);
    mdi->mocked_data.set_le64(offset + 32, start);
    mdi->mocked_data.set_le64(offset + 40, end);
    GetProtectedGptMembers::do_set_name(mdi->mocked_data, offset + 56, name);
}

}

namespace testing {

TEST(GptPartitionTableTests, ByteBufferToNameConverstion) {
    ByteBuffer bb = std::vector<uint8_t>{
    /*0x00*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x10*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x20*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x30*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x40*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x50*/  'T', 0x00, 'E', 0x00, 'S', 0x00, 'T', 0x00, 'N', 0x00, 'A', 0x00, 'M', 0x00, 'E', 0x00,
    /*0x60*/  't', 0x00, 'e', 0x00, 's', 0x00, 't', 0x00, 'n', 0x00, 'a', 0x00, 'm', 0x00, 'e', 0x00,
    /*0x70*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x80*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x90*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0xA0*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0xB0*/  '1', 0x00, '2', 0x00, '3', 0x00, '4', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x00), "");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x10), "TEST");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x18), "TESTNAME");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x20), "TESTNAMEtest");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x40), "TESTNAMEtestname");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x6A), "ame1");
    EXPECT_EQ(GetProtectedGptMembers::do_get_name(bb, 0x70), "1234");
}

TEST(GptPartitionTableTests, NameToByteBufferConverstion) {

    static constexpr int BUF_OFFSET = 0x10;
    ByteBuffer bb_original(16 * 6); // size of all buffers in this test
    for (auto& ch : bb_original) {
        ch = 0x11;
    }

    std::string name1 = "abcd";
    ByteBuffer expected_bb1 = std::vector<uint8_t>{
    /*0x00*/  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    /*0x10*/  'a', 0x00, 'b', 0x00, 'c', 0x00, 'd', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x20*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x30*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x40*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x50*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    };
    auto bb_test1 = bb_original;
    GetProtectedGptMembers::do_set_name(bb_test1, BUF_OFFSET, name1);
    for (unsigned i = 0; i < bb_test1.size(); ++i) {
        EXPECT_EQ(bb_test1[i], expected_bb1[i]);
    }

    std::string name2 = "string_that_is_longer_than_36_characters";
    ByteBuffer expected_bb2 = std::vector<uint8_t>{
    /*0x00*/  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    /*0x10*/  's', 0x00, 't', 0x00, 'r', 0x00, 'i', 0x00, 'n', 0x00, 'g', 0x00, '_', 0x00, 't', 0x00,
    /*0x20*/  'h', 0x00, 'a', 0x00, 't', 0x00, '_', 0x00, 'i', 0x00, 's', 0x00, '_', 0x00, 'l', 0x00,
    /*0x30*/  'o', 0x00, 'n', 0x00, 'g', 0x00, 'e', 0x00, 'r', 0x00, '_', 0x00, 't', 0x00, 'h', 0x00,
    /*0x40*/  'a', 0x00, 'n', 0x00, '_', 0x00, '3', 0x00, '6', 0x00, '_', 0x00, 'c', 0x00, 'h', 0x00,
    /*0x50*/  'a', 0x00, 'r', 0x00, 'a', 0x00, 'c', 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    };
    auto bb_test2 = bb_original;
    GetProtectedGptMembers::do_set_name(bb_test2, BUF_OFFSET, name2);
    for (unsigned i = 0; i < bb_test2.size(); ++i) {
        EXPECT_EQ(bb_test2[i], expected_bb2[i]);
    }

    std::string name3 = "";
    ByteBuffer expected_bb3 = std::vector<uint8_t>{
    /*0x00*/  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    /*0x10*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x20*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x30*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x40*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0x50*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
    };
    auto bb_test3 = bb_original;
    GetProtectedGptMembers::do_set_name(bb_test3, BUF_OFFSET, name3);
    for (unsigned i = 0; i < bb_test3.size(); ++i) {
        EXPECT_EQ(bb_test3[i], expected_bb3[i]);
    }
}

TEST(GptPartitionTableTests, ByteBufferToGuidConverstion) {
    ByteBuffer bb = std::vector<uint8_t>{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xaf, 0x3d, 0xc6, 0x0f, 0x83, 0x84, 0x72, 0x47, 0x8e, 0x79, 0x3d, 0x69, 0xd8, 0x47, 0x7d, 0xe4,
        0x6b, 0x7a, 0x30, 0x83, 0x73, 0x6a, 0x82, 0x49, 0xac, 0x65, 0xd2, 0x9e, 0x53, 0xf4, 0x6e, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    EXPECT_EQ(GetProtectedGptMembers::do_get_guid(bb, 0x10), "0FC63DAF-8483-4772-8E79-3D69D8477DE4");
    EXPECT_EQ(GetProtectedGptMembers::do_get_guid(bb, 0x18), "693D798E-47D8-E47D-6B7A-3083736A8249");
    EXPECT_EQ(GetProtectedGptMembers::do_get_guid(bb, 0x20), "83307A6B-6A73-4982-AC65-D29E53F46E40");
    EXPECT_EQ(GetProtectedGptMembers::do_get_guid(bb, 0x30), "00000000-0000-0000-0000-000000000000");
}

TEST(GptPartitionTableTests, GuidToByteBufferConverstion) {
    ByteBuffer expected = std::vector<uint8_t>{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xaf, 0x3d, 0xc6, 0x0f, 0x83, 0x84, 0x72, 0x47, 0x8e, 0x79, 0x3d, 0x69, 0xd8, 0x47, 0x7d, 0xe4,
        0x6b, 0x7a, 0x30, 0x83, 0x73, 0x6a, 0x82, 0x49, 0xac, 0x65, 0xd2, 0x9e, 0x53, 0xf4, 0x6e, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    ByteBuffer test(expected.size());
    GetProtectedGptMembers::do_set_guid(test, 0x10, "0FC63DAF-8483-4772-8E79-3D69D8477DE4");
    GetProtectedGptMembers::do_set_guid(test, 0x20, "83307A6B-6A73-4982-AC65-D29E53F46E40");
    GetProtectedGptMembers::do_set_guid(test, 0x30, "00000000-0000-0000-0000-000000000000");

    for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], test[i]);
    }

    try {
        GetProtectedGptMembers::do_set_guid(test, 0x10, "0FC63DAF-8483-4772-8E793D69D8477DE4"); // missing '-'
        FAIL() << "No exception was thrown";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type thrown";
    }

    try {
        GetProtectedGptMembers::do_set_guid(test, 0x10, "0FC63DAF-8483-4772-8E793g69D8477DE4"); // invalid character 'g'
        FAIL() << "No exception was thrown";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type thrown";
    }

    try {
        GetProtectedGptMembers::do_set_guid(test, 0x10, "0FC63DAF-8483-4772-8E793D69D8477DE42"); // too long
        FAIL() << "No exception was thrown";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type thrown";
    }


    // check if invalid cases did not modify the buffer
    for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], test[i]);
    }
}

TEST(GptPartitionTableTests, ByteBufferToGptHeaderConverstion) {
    ByteBuffer bb = std::vector<uint8_t>{
    /* 0x0000 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x10, 0x11, 0x12, 0x13, 0x20, 0x21, 0x22, 0x23,
    /* 0x0010 */ 0x30, 0x31, 0x32, 0x33, 0x3A, 0x3A, 0x3A, 0x3A, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    /* 0x0020 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    /* 0x0030 */ 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    /* 0x0040 */ 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    /* 0x0050 */ 0xA0, 0xA1, 0xA2, 0xA3, 0xB0, 0xB1, 0xB2, 0xB3, 0xC0, 0xC1, 0xC2, 0xC3
    };

    auto gpt = GetProtectedGptMembers::do_bb_to_gpt_header(bb);
    EXPECT_EQ(gpt.signature, 0x0807060504030201);
    EXPECT_EQ(gpt.revision, 0x13121110);
    EXPECT_EQ(gpt.header_size, 0x23222120);
    EXPECT_EQ(gpt.header_crc, 0x33323130);
    EXPECT_EQ(gpt.current_lba, 0x4746454443424140);
    EXPECT_EQ(gpt.backup_lba, 0x5756555453525150);
    EXPECT_EQ(gpt.first_usable_lba, 0x6766656463626160);
    EXPECT_EQ(gpt.last_usable_lba, 0x7776757473727170);
    EXPECT_EQ(gpt.disk_guid, "83828180-8584-8786-8889-8A8B8C8D8E8F");
    EXPECT_EQ(gpt.partition_table_lba, 0x9796959493929190);
    EXPECT_EQ(gpt.num_of_partitions, 0xA3A2A1A0);
    EXPECT_EQ(gpt.partition_entry_size, 0xB3B2B1B0);
    EXPECT_EQ(gpt.partition_table_crc, 0xC3C2C1C0);
}

TEST(GptPartitionTableTests, ByteBufferToGptTableConverstion) {
    // first partition entry is empty, the second one is filled with data
    ByteBuffer bb = std::vector<uint8_t>{
    /* 0x0000 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0010 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0020 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0030 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0040 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0050 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0060 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0070 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x0080 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    /* 0x0090 */ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    /* 0x00A0 */ 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    /* 0x00B0 */ 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x20, 0x00, 0x54, 0x00, 0x45, 0x00, 0x53, 0x00,
    /* 0x00C0 */ 0x54, 0x00, 0x5F, 0x00, 0x50, 0x00, 0x41, 0x00, 0x52, 0x00, 0x54, 0x00, 0x49, 0x00, 0x54, 0x00,
    /* 0x00D0 */ 0x49, 0x00, 0x4F, 0x00, 0x4E, 0x00, 0x5F, 0x00, 0x4E, 0x00, 0x41, 0x00, 0x4D, 0x00, 0x45, 0x00,
    /* 0x00E0 */ 0x20, 0x00, 0x54, 0x00, 0x45, 0x00, 0x53, 0x00, 0x54, 0x00, 0x5F, 0x00, 0x50, 0x00, 0x41, 0x00,
    /* 0x00F0 */ 0x52, 0x00, 0x54, 0x00, 0x49, 0x00, 0x54, 0x00, 0x49, 0x00, 0x4F, 0x00, 0x4E, 0x00, 0x5F, 0x00,
    };

    /*! Fill whole buffer with the same partition */
    bb.get_raw_vector().resize(EXPECTED_GPT_PARTITION_COUNT * EXPECTED_GPT_PARTITION_SIZE);
    for (unsigned i = 2; i < EXPECTED_GPT_PARTITION_COUNT; ++i) {
        for (unsigned j = 0; j < EXPECTED_GPT_PARTITION_SIZE; ++j) {
            bb[i * EXPECTED_GPT_PARTITION_COUNT + j] = bb[j + EXPECTED_GPT_PARTITION_SIZE];
        }
    }

    auto gpt = GetProtectedGptMembers::do_bb_to_gpt_table(bb);
    // no point of checking the partition id = 0, we start at 1
    for (unsigned i = 1; i < EXPECTED_GPT_PARTITION_COUNT; ++i) {
        EXPECT_EQ(gpt[i].type_guid, "03020100-0504-0706-0809-0A0B0C0D0E0F");
        EXPECT_EQ(gpt[i].guid, "13121110-1514-1716-1819-1A1B1C1D1E1F");
        EXPECT_EQ(gpt[i].first_lba, 0x2827262524232221);
        EXPECT_EQ(gpt[i].last_lba, 0x3837363534333231);
        EXPECT_EQ(gpt[i].flags, 0x4847464544434241);
        EXPECT_EQ(gpt[i].name, " TEST_PARTITION_NAME TEST_PARTITION_");
    }
}

TEST(GptPartitionTableTests, GptHeaderToByteBufferConverstion) {
    GetProtectedGptMembers::GptHeaderData gpt{};
    gpt.signature = 0x0807060504030201;
    gpt.revision = 0x13121110;
    gpt.header_size = 0x23222120;
    gpt.header_crc = 0x33323130;
    gpt.current_lba = 0x4746454443424140;
    gpt.backup_lba = 0x5756555453525150;
    gpt.first_usable_lba = 0x6766656463626160;
    gpt.last_usable_lba = 0x7776757473727170;
    gpt.disk_guid = "83828180-8584-8786-8889-8A8B8C8D8E8F";
    gpt.partition_table_lba = 0x9796959493929190;
    gpt.num_of_partitions = 0xA3A2A1A0;
    gpt.partition_entry_size = 0xB3B2B1B0;
    gpt.partition_table_crc = 0xC3C2C1C0;
    auto bb = GetProtectedGptMembers::do_gpt_header_to_bb(gpt);

    ByteBuffer expected = std::vector<uint8_t>{
    /* 0x0000 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x10, 0x11, 0x12, 0x13, 0x20, 0x21, 0x22, 0x23,
    /* 0x0010 */ 0x30, 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    /* 0x0020 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    /* 0x0030 */ 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    /* 0x0040 */ 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    /* 0x0050 */ 0xA0, 0xA1, 0xA2, 0xA3, 0xB0, 0xB1, 0xB2, 0xB3, 0xC0, 0xC1, 0xC2, 0xC3
    };

    for (unsigned i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], bb[i]);
    }
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunsafe-loop-optimizations"
#endif

TEST(GptPartitionTableTests, GptTableToByteBufferConverstion) {
    GetProtectedGptMembers::GptTableData gpt{};
    gpt[1].type_guid = "03020100-0504-0706-0809-0A0B0C0D0E0F";
    gpt[1].guid = "13121110-1514-1716-1819-1A1B1C1D1E1F";
    gpt[1].first_lba = 0x2827262524232221;
    gpt[1].last_lba = 0x3837363534333231;
    gpt[1].flags = 0x4847464544434241;
    gpt[1].name = " TEST_PARTITION_NAME TEST_PARTITION_";
    auto bb = GetProtectedGptMembers::do_gpt_table_to_bb(gpt);

    ByteBuffer expected = std::vector<uint8_t>{
    /* 0x0000 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    /* 0x0010 */ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    /* 0x0020 */ 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    /* 0x0030 */ 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, ' ', 0x00, 'T', 0x00, 'E', 0x00, 'S', 0x00,
    /* 0x0040 */ 'T', 0x00, '_', 0x00, 'P', 0x00, 'A', 0x00, 'R', 0x00, 'T', 0x00, 'I', 0x00, 'T', 0x00,
    /* 0x0050 */ 'I', 0x00, 'O', 0x00, 'N', 0x00, '_', 0x00, 'N', 0x00, 'A', 0x00, 'M', 0x00, 'E', 0x00,
    /* 0x0060 */ ' ', 0x00, 'T', 0x00, 'E', 0x00, 'S', 0x00, 'T', 0x00, '_', 0x00, 'P', 0x00, 'A', 0x00,
    /* 0x0070 */ 'R', 0x00, 'T', 0x00, 'I', 0x00, 'T', 0x00, 'I', 0x00, 'O', 0x00, 'N', 0x00, '_', 0x00
    };
    expected.get_raw_vector().resize(EXPECTED_GPT_PARTITION_SIZE * EXPECTED_GPT_PARTITION_COUNT);

    for (unsigned i = 0; i < EXPECTED_GPT_PARTITION_SIZE; ++i) {
        EXPECT_EQ(bb[i], 0x00);
    }
    for (unsigned i = 0; i < EXPECTED_GPT_PARTITION_SIZE; ++i) {
        EXPECT_EQ(bb[i], 0x00);
        EXPECT_EQ(bb[i + EXPECTED_GPT_PARTITION_SIZE], expected[i]);
    }
    for (unsigned i = EXPECTED_GPT_PARTITION_SIZE * 2; i < EXPECTED_GPT_PARTITION_SIZE * EXPECTED_GPT_PARTITION_COUNT; ++i) {
        EXPECT_EQ(bb[i], 0x00);
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

TEST(GptPartitionTableTests, LoadOnEmptyDrive) {
    auto ptr = std::make_shared<MockDriveInterface>();
    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);
    // should be invalid - no valid mbr
    EXPECT_EQ(gpt.is_valid(), false);
}

TEST(GptPartitionTableTests, LoadOnEmptyMbr) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(EXPECTED_MBR_SIZE);

    // create a valid, empty mbr
    MbrPartitionTable mbr{};
    mbr.reset(100);
    mbr.update();
    mbr.save({}, ptr);

    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);

    // should be invalid - mbr valid, but no partitions (mbr protective partition is required)
    EXPECT_EQ(gpt.is_valid(), false);
}

TEST(GptPartitionTableTests, LoadOnMbrButNoGpt) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(EXPECTED_MBR_SIZE);

    // create a valid mbr with mbr protective partition
    make_mbr(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);

    // should be invalid - mbr valid + mbr protective partition, but no gpt
    EXPECT_EQ(gpt.is_valid(), false);
}

TEST(GptPartitionTableTests, LoadOnEmptyGpt) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    // should be valid, there is no gpt backupt but it should not be checked as primary table is ok
    EXPECT_EQ(gpt.is_valid(), true);
}

TEST(GptPartitionTableTests, LoadOnEmptyGptWithInvalidHeaderCrcAndInvalidBackup) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x12345678);

    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);

    // this should fail as header CRC is invalid (while table CRC is correct) and there is no backup
    EXPECT_EQ(gpt.is_valid(), false);
}

TEST(GptPartitionTableTests, LoadOnEmptyGptWithInvalidTableCrcAndInvalidBackup) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    // 0xAB54D286 would be ok, this is an invalid table CRC
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, 0xAB54D287);
    // this is a valid CRC for the header with the updated table CRC
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x25805EBD);

    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);

    // this should fail as table CRC is invalid (while header CRC is correct) and there is no backup
    EXPECT_EQ(gpt.is_valid(), false);
}


TEST(GptPartitionTableTests, LoadOnEmptyGptWithInvalidHeaderCrcAndValidBackup) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    update_gpt_backup(ptr);
    update_backup_crcs(ptr);
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x12345678);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    // header CRC is invalid, but there is a valid backup GPT, so should be ok
    EXPECT_EQ(gpt.is_valid(), true);
}

TEST(GptPartitionTableTests, LoadOnEmptyGptWithInvalidTableCrcAndValidBackup) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    update_gpt_backup(ptr);
    update_backup_crcs(ptr);
    // 0xAB54D286 would be ok, this is an invalid table CRC
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, 0xAB54D287);
    // this is a valid CRC for the header with the updated table CRC
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x25805EBD);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    // table CRC is invalid, but there is a valid backup GPT, so should be ok
    EXPECT_EQ(gpt.is_valid(), true);
}

TEST(GptPartitionTableTests, LoadTestOnRealData) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(RAW_DEVICE_DATA_SIZE);
    std::copy(std::begin(RAW_DEVICE_DATA),
              std::begin(RAW_DEVICE_DATA) + RAW_DEVICE_DATA_SIZE,
              ptr->mocked_data.get_raw_vector().begin());
    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_TRUE(gpt.is_valid());

    std::vector<unsigned> ids(25);
    std::iota(ids.begin(), ids.end(), 0);  // generates [0, ... , 24]
    EXPECT_EQ(ids, gpt.get_partition_ids());

    std::vector<std::string> expected_names = {"P1", "P2", "P3", "test_1", "Part5", "Part6", "Part7", "Part8", "Part9",
        "New_partition", "test_2", "test_3", "test_4", "test_5", "test_6", "test_7", "test_8", "test_9", "test_10",
        "test_11", "test_12", "test_13", "test_14", "test_15", "test_16"};
    std::vector<uint64_t> expected_first_lbas = {34, 1953792, 5859328, 39062500, 9764864, 11718656, 13672448, 15624192,
        17577984, 19531776, 58593751, 78125001, 97656250, 97656249, 97656251, 97656252, 97656253, 97656254, 97656255,
        97656256, 97656257, 97656258, 97656259, 97656260, 97656261};
    std::vector<uint64_t> expected_size_lbas = {1953092, 1951744, 1953792, 19531251, 1953792, 1953792, 1951744, 1953792,
        1953792, 1, 19531250, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (unsigned i = 0; i < ids.size(); ++i) {
        auto pd = gpt.get_partition(i);
        EXPECT_EQ(pd.name, expected_names[i]);
        EXPECT_EQ(pd.type, PartitionType::LINUX);
        EXPECT_EQ(pd.first_lba, expected_first_lbas[i]);
        EXPECT_EQ(pd.size_lba, expected_size_lbas[i]);
    }
}

TEST(GptPartitionTableTests, NonInitializedGptTableCannotBeSaved) {
    auto ptr = std::make_shared<MockDriveInterface>();
    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_EQ(gpt.is_valid(), false);
    EXPECT_THROW(gpt.save({}, ptr), std::runtime_error);
}

TEST(GptPartitionTableTests, InvalidGptTableCannotBeSaved) {
    auto ptr = std::make_shared<MockDriveInterface>();
    // create an invalid gpt table and load it, gpt table should be filled with some data, but still be invalid
    make_gpt(ptr);
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x12345678);
    GptPartitionTable gpt(LBA_SIZE);
    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);

    EXPECT_EQ(gpt.is_valid(), false);
    EXPECT_THROW(gpt.save({}, ptr), std::runtime_error);
}

TEST(GptPartitionTableTests, GetPartitionIdsEmptyDrive) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ((std::vector<unsigned>{}), gpt.get_partition_ids());
    EXPECT_EQ(0, gpt.get_available_partition_id());
}

TEST(GptPartitionTableTests, GetPartitionIdsNonEmptyDriveCase1) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    add_partition(ptr, 0);
    update_crcs(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ((std::vector<unsigned>{0}), gpt.get_partition_ids());
    EXPECT_EQ(1, gpt.get_available_partition_id());
}

TEST(GptPartitionTableTests, GetPartitionIdsNonEmptyDriveCase2) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    add_partition(ptr, 0);
    add_partition(ptr, 2);
    add_partition(ptr, 4);
    update_crcs(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ((std::vector<unsigned>{0, 2, 4}), gpt.get_partition_ids());
    EXPECT_EQ(1, gpt.get_available_partition_id());
}

TEST(GptPartitionTableTests, GetPartitionIdsNonEmptyDriveCase3) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    for (unsigned i = 0; i < 10; ++i) {
        add_partition(ptr, i);
    }
    update_crcs(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ((std::vector<unsigned>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), gpt.get_partition_ids());
    EXPECT_EQ(10, gpt.get_available_partition_id());
}

TEST(GptPartitionTableTests, GetPartitionIdsFullDrive) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    std::vector<unsigned> expected{};
    for (unsigned i = 0; i < EXPECTED_GPT_PARTITION_COUNT; ++i) {
        add_partition(ptr, i);
        expected.push_back(i);
    }
    update_crcs(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ(expected, gpt.get_partition_ids());
    try {
        gpt.get_available_partition_id();
        FAIL() << "Expected exception not thrown";
    }
    catch (const GptPartitionTable::IdUnavailable&) {
        // expected
    }
    catch (...) {
        FAIL() << "Unexpected exception type was thrown";
    }
}

TEST(GptPartitionTableTests, GetPartitionTest) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    add_partition(ptr, 1, "Name1", EXPECTED_LINUX_PARTITION_GUID, 0, 100);
    add_partition(ptr, 2, "", EXPECTED_LINUX_PARTITION_GUID, 0, 0);
    add_partition(ptr, 3, "Name2", "12345678-1234-1234-1234-123456789ABC", 0, 200);
    add_partition(ptr, 4, "Very long name", EXPECTED_LINUX_PARTITION_GUID, 100, 100);
    add_partition(ptr, 5, "Name that is much longer than 36 characters", EXPECTED_LINUX_PARTITION_GUID, 100, 200);
    add_partition(ptr, 6, "Name", EXPECTED_LINUX_PARTITION_GUID, 200, 100);
    update_crcs(ptr);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    EXPECT_EQ((std::vector<unsigned>{1, 2, 3, 4, 5, 6}), gpt.get_partition_ids());
    EXPECT_EQ(0, gpt.get_available_partition_id());

    auto partition = gpt.get_partition(1);
    EXPECT_EQ(partition.name, "Name1");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 101);

    partition = gpt.get_partition(2);
    EXPECT_EQ(partition.name, "");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 1);

    partition = gpt.get_partition(3);
    EXPECT_EQ(partition.name, "Name2");
    EXPECT_EQ(partition.type, PartitionType::UNKNOWN);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 201);

    partition = gpt.get_partition(4);
    EXPECT_EQ(partition.name, "Very long name");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 100);
    EXPECT_EQ(partition.size_lba, 1);

    partition = gpt.get_partition(5);
    EXPECT_EQ(partition.name, "Name that is much longer than 36 cha");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 100);
    EXPECT_EQ(partition.size_lba, 101);

    // last LBA < first LBA, should throw
    EXPECT_THROW(gpt.get_partition(6), std::runtime_error);

    EXPECT_THROW(gpt.get_partition(7), std::runtime_error);
    EXPECT_THROW(gpt.get_partition(7), std::runtime_error);
    EXPECT_THROW(gpt.get_partition(99999), std::runtime_error);

}

TEST(GptPartitionTableTests, GetPartitionTestLoadFromBackupTable) {
    auto ptr = std::make_shared<MockDriveInterface>();
    // create a valid GPT, add partitions
    make_gpt(ptr);
    add_partition(ptr, 1, "Name1", EXPECTED_LINUX_PARTITION_GUID, 0, 100);
    add_partition(ptr, 2, "", EXPECTED_LINUX_PARTITION_GUID, 0, 0);
    add_partition(ptr, 3, "Name2", "12345678-1234-1234-1234-123456789ABC", 0, 200);
    add_partition(ptr, 4, "Very long name", EXPECTED_LINUX_PARTITION_GUID, 100, 100);
    add_partition(ptr, 5, "Name that is much longer than 36 characters", EXPECTED_LINUX_PARTITION_GUID, 100, 200);
    add_partition(ptr, 6, "Name", EXPECTED_LINUX_PARTITION_GUID, 200, 100);
    // update CRCs and create a backup table
    update_crcs(ptr);
    update_gpt_backup(ptr);
    update_backup_crcs(ptr);
    // generate a default GPT again (no partitions in the primary table)
    make_gpt(ptr);
    // modify primary GPT so it is invalid, it will force algorithm to read the backup table
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, 0x11111111);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);
    EXPECT_EQ(gpt.is_valid(), true);

    // we expect to read the same entries as in the previous test, as the backup table included all partitions

    EXPECT_EQ((std::vector<unsigned>{1, 2, 3, 4, 5, 6}), gpt.get_partition_ids());
    EXPECT_EQ(0, gpt.get_available_partition_id());

    auto partition = gpt.get_partition(1);
    EXPECT_EQ(partition.name, "Name1");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 101);

    partition = gpt.get_partition(2);
    EXPECT_EQ(partition.name, "");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 1);

    partition = gpt.get_partition(3);
    EXPECT_EQ(partition.name, "Name2");
    EXPECT_EQ(partition.type, PartitionType::UNKNOWN);
    EXPECT_EQ(partition.first_lba, 0);
    EXPECT_EQ(partition.size_lba, 201);

    partition = gpt.get_partition(4);
    EXPECT_EQ(partition.name, "Very long name");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 100);
    EXPECT_EQ(partition.size_lba, 1);

    partition = gpt.get_partition(5);
    EXPECT_EQ(partition.name, "Name that is much longer than 36 cha");
    EXPECT_EQ(partition.type, PartitionType::LINUX);
    EXPECT_EQ(partition.first_lba, 100);
    EXPECT_EQ(partition.size_lba, 101);

    // last LBA < first LBA, should throw
    EXPECT_THROW(gpt.get_partition(6), std::runtime_error);

    EXPECT_THROW(gpt.get_partition(7), std::runtime_error);
    EXPECT_THROW(gpt.get_partition(7), std::runtime_error);
    EXPECT_THROW(gpt.get_partition(99999), std::runtime_error);
}


TEST(GptPartitionTableTests, GetPartitionTestInvalidId) {
    auto ptr = std::make_shared<MockDriveInterface>();
    make_gpt(ptr);
    GptPartitionTable gpt(LBA_SIZE);
    gpt.load({}, ptr);

    try {
        gpt.get_partition(EXPECTED_GPT_PARTITION_COUNT);
        FAIL() << "Expected exception not thrown";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Unexpected exception type was thrown";
    }
}

TEST(GptPartitionTableTests, SaveEmptyGptPartitionTable) {

    // prepare reset gpt table
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);
    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA); // reset should produce a valid partition table
    ASSERT_TRUE(gpt.is_valid());
    gpt.save({}, ptr);

    // extract stored data
    ByteBuffer primary_header_buffer{};
    ByteBuffer backup_header_buffer{};
    primary_header_buffer.get_raw_vector() = {
        ptr->mocked_data.begin() + TEST_PRIMARY_HEADER_LBA * LBA_SIZE,
        ptr->mocked_data.begin() + TEST_PRIMARY_HEADER_LBA * LBA_SIZE + EXPECTED_GPT_HEADER_SIZE
    };
    backup_header_buffer.get_raw_vector() = {
        ptr->mocked_data.begin() + TEST_BACKUP_HEADER_LBA * LBA_SIZE,
        ptr->mocked_data.begin() + TEST_BACKUP_HEADER_LBA * LBA_SIZE + EXPECTED_GPT_HEADER_SIZE
    };

    // compare primary header values with expectations
    auto header = GetProtectedGptMembers::do_bb_to_gpt_header(primary_header_buffer);
    EXPECT_EQ(header.signature, EXPECTED_GPT_SIGNATURE);
    EXPECT_EQ(header.revision, EXPECTED_GPT_REVISION);
    EXPECT_EQ(header.header_size, EXPECTED_GPT_HEADER_SIZE);
    EXPECT_EQ(header.current_lba, TEST_PRIMARY_HEADER_LBA);
    EXPECT_EQ(header.backup_lba, TEST_BACKUP_HEADER_LBA);
    EXPECT_EQ(header.first_usable_lba, TEST_FIRST_USABLE_LBA);
    EXPECT_EQ(header.last_usable_lba, TEST_LAST_USABLE_LBA);
    EXPECT_TRUE(!header.disk_guid.empty());
    EXPECT_EQ(header.partition_table_lba, TEST_PRIMARY_TABLE_LBA);
    EXPECT_EQ(header.num_of_partitions, EXPECTED_GPT_PARTITION_COUNT);
    EXPECT_EQ(header.partition_entry_size, EXPECTED_GPT_PARTITION_SIZE);

    // compare backup header values with expectations
    auto backup = GetProtectedGptMembers::do_bb_to_gpt_header(backup_header_buffer);
    EXPECT_EQ(backup.signature, EXPECTED_GPT_SIGNATURE);
    EXPECT_EQ(backup.revision, EXPECTED_GPT_REVISION);
    EXPECT_EQ(backup.header_size, EXPECTED_GPT_HEADER_SIZE);
    EXPECT_EQ(backup.current_lba, TEST_BACKUP_HEADER_LBA);
    EXPECT_EQ(backup.backup_lba, TEST_PRIMARY_HEADER_LBA);
    EXPECT_EQ(backup.first_usable_lba, TEST_FIRST_USABLE_LBA);
    EXPECT_EQ(backup.last_usable_lba, TEST_LAST_USABLE_LBA);
    EXPECT_TRUE(!backup.disk_guid.empty());
    EXPECT_EQ(backup.partition_table_lba, TEST_LAST_USABLE_LBA + 1);
    EXPECT_EQ(backup.num_of_partitions, EXPECTED_GPT_PARTITION_COUNT);
    EXPECT_EQ(backup.partition_entry_size, EXPECTED_GPT_PARTITION_SIZE);
}

TEST(GptPartitionTableTests, UpdateRepairsPartitionTable) {

    constexpr uint64_t total_drive_size_lba = 80;

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(total_drive_size_lba * LBA_SIZE);
    GptPartitionTable gpt(LBA_SIZE);

    // modify header, disk guid
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 56, 0x12345678);
    // modify header, header crc
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 16, 0x12345678);
    // modify header, table crc
    ptr->mocked_data.set_le32(TEST_PRIMARY_HEADER_LBA * LBA_SIZE + 88, 0x12345678);
    // modify partition table, first lba
    ptr->mocked_data.set_le32(TEST_PRIMARY_TABLE_LBA * LBA_SIZE + 32, 0x12345678);

    EXPECT_THROW(gpt.load({}, ptr), std::runtime_error);
    // this should fail as location of the backup header is not filled and primary header is invalid
    ASSERT_FALSE(gpt.is_valid());
    gpt.update();
    EXPECT_TRUE(gpt.is_valid());
}

TEST(GptPartitionTableTests, GptTableIsAbleToLoadItsOwnTables) {

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);
    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);
    PartitionData pd{};
    pd.type = PartitionType::LINUX;
    pd.first_lba = 35;
    pd.size_lba = 10;
    pd.name = "test";
    gpt.set_partition(0, pd);
    gpt.update();
    EXPECT_TRUE(gpt.is_valid());
    gpt.save({}, ptr);
    gpt.load({}, ptr);
    EXPECT_TRUE(gpt.is_valid());
}

TEST(GptPartitionTableTests, SetRemoveTests) {

    // prepare reset gpt table
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);
    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA); // reset should produce a valid partition table
    ASSERT_TRUE(gpt.is_valid());
    gpt.save({}, ptr);

    PartitionData pd{};
    pd.type = PartitionType::MBR_PROTECTIVE;
    pd.first_lba = 35;
    pd.size_lba = 10;
    pd.name = "test";

    // invalid types
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);
    pd.type = PartitionType::UNKNOWN;
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);

    pd.type = PartitionType::LINUX;
    // this should work
    gpt.set_partition(11, pd);
    EXPECT_EQ((std::vector<unsigned>{11}), gpt.get_partition_ids());

    gpt.remove_partition(11);
    // should not fail if done again
    gpt.remove_partition(11);
    EXPECT_EQ((std::vector<unsigned>{}), gpt.get_partition_ids());


    pd.name = "Very long name that is definitely too long for this partition table";
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);
    pd.name = "test";
    gpt.set_partition(11, pd);
    gpt.remove_partition(11);

    // too large ids
    EXPECT_THROW(gpt.remove_partition(99999), std::runtime_error);
    EXPECT_THROW(gpt.set_partition(99999, pd), std::runtime_error);
    EXPECT_EQ((std::vector<unsigned>{}), gpt.get_partition_ids());

    pd.size_lba = 0;
    // invalid size
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);
    pd.size_lba = 2;

    // partition on primary/backup GPT table
    pd.first_lba = 3;
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);
    pd.first_lba = TEST_BACKUP_TABLE_LBA + 2;
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);
    pd.first_lba = TEST_BACKUP_TABLE_LBA - 1;
    EXPECT_THROW(gpt.set_partition(11, pd), std::runtime_error);

    pd.first_lba = 45;   // LBAs [45, 46, 47, 48, 49]
    pd.size_lba = 5;
    gpt.set_partition(11, pd);

    // check overlaps
    EXPECT_THROW(gpt.set_partition(12, pd), std::runtime_error);
    pd.first_lba = 44;  // LBAs (44, 45)
    pd.size_lba = 2;
    EXPECT_THROW(gpt.set_partition(12, pd), std::runtime_error);
    pd.first_lba = 45;  // LBAs (45, 46)
    EXPECT_THROW(gpt.set_partition(12, pd), std::runtime_error);
    pd.first_lba = 49;  // LBAs (49, 50)
    EXPECT_THROW(gpt.set_partition(12, pd), std::runtime_error);
    pd.first_lba = 44;  // LBAs (44, 45, 46, 47, 48, 49, 50)
    pd.size_lba = 7;
    EXPECT_THROW(gpt.set_partition(12, pd), std::runtime_error);

    // check if creating partitions on 'borders' work
    pd.first_lba = 44;
    pd.size_lba = 1;
    gpt.set_partition(20, pd);
    pd.first_lba = 50;
    pd.size_lba = 1;
    gpt.set_partition(21, pd);
    EXPECT_EQ((std::vector<unsigned>{11, 20, 21}), gpt.get_partition_ids());

}

TEST(GptPartitionTableTests, SaveCorrectlyStoresPartitionData) {

    // prepare reset gpt table
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);
    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA); // reset should produce a valid partition table
    ASSERT_TRUE(gpt.is_valid());

    PartitionData pd{};
    pd.type = PartitionType::LINUX;
    pd.first_lba = 35;
    pd.size_lba = 10;
    pd.name = "first";
    gpt.set_partition(0, pd);
    pd.first_lba = 45;
    pd.size_lba = 5;
    pd.name = "middle";
    gpt.set_partition(66, pd);
    pd.first_lba = 55;
    pd.size_lba = 2;
    pd.name = "last";
    gpt.set_partition(EXPECTED_GPT_PARTITION_COUNT - 1, pd);
    gpt.update();
    gpt.save({}, ptr);

    // extract stored data
    ByteBuffer primary_table_buffer{};
    ByteBuffer backup_table_buffer{};
    primary_table_buffer.get_raw_vector() = {
        ptr->mocked_data.begin() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE,
        ptr->mocked_data.begin() + TEST_PRIMARY_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_TABLE_SIZE
    };
    backup_table_buffer.get_raw_vector() = {
        ptr->mocked_data.begin() + TEST_BACKUP_TABLE_LBA * LBA_SIZE,
        ptr->mocked_data.begin() + TEST_BACKUP_TABLE_LBA * LBA_SIZE + EXPECTED_GPT_TABLE_SIZE
    };
    // primary and backup buffers should be the same
    EXPECT_EQ(primary_table_buffer.get_raw_vector(), backup_table_buffer.get_raw_vector());

    // not set entries should be empty
    for (unsigned i = 0; i < EXPECTED_GPT_PARTITION_COUNT; ++i) {
        if (i == 0 || i == 66 || i == EXPECTED_GPT_PARTITION_COUNT - 1) {
            continue;
        }
        for (unsigned j = 0; j < EXPECTED_GPT_PARTITION_SIZE; ++j) {
            EXPECT_EQ(0, primary_table_buffer[i * EXPECTED_GPT_PARTITION_SIZE + j]);
        }
    }

    auto table = GetProtectedGptMembers::do_bb_to_gpt_table(primary_table_buffer);

    EXPECT_EQ(table[0].type_guid, EXPECTED_LINUX_PARTITION_GUID);
    EXPECT_FALSE(table[0].guid.empty());
    EXPECT_EQ(table[0].name, "first");
    EXPECT_EQ(table[0].first_lba, 35);
    EXPECT_EQ(table[0].last_lba, 44);

    EXPECT_EQ(table[66].type_guid, EXPECTED_LINUX_PARTITION_GUID);
    EXPECT_FALSE(table[66].guid.empty());
    EXPECT_EQ(table[66].name, "middle");
    EXPECT_EQ(table[66].first_lba, 45);
    EXPECT_EQ(table[66].last_lba, 49);

    EXPECT_EQ(table[EXPECTED_GPT_PARTITION_COUNT - 1].type_guid, EXPECTED_LINUX_PARTITION_GUID);
    EXPECT_FALSE(table[EXPECTED_GPT_PARTITION_COUNT - 1].guid.empty());
    EXPECT_EQ(table[EXPECTED_GPT_PARTITION_COUNT - 1].name, "last");
    EXPECT_EQ(table[EXPECTED_GPT_PARTITION_COUNT - 1].first_lba, 55);
    EXPECT_EQ(table[EXPECTED_GPT_PARTITION_COUNT - 1].last_lba, 56);
}

TEST(GptPartitionTableTests, GetEmptyRegionEmptyDriveTests) {
    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    // for shorter expressions
    static constexpr auto START = TEST_FIRST_USABLE_LBA;
    static constexpr auto END = TEST_LAST_USABLE_LBA;
    static constexpr auto MAX_SIZE = END - START + 1;

    // EMPTY GPT TESTS
    // invalid parameter
    EXPECT_THROW(gpt.get_empty_region(0), std::runtime_error);
    EXPECT_EQ(START, gpt.get_empty_region(1));
    EXPECT_EQ(START, gpt.get_empty_region(10));
    // maximum possible on this drive
    EXPECT_EQ(START, gpt.get_empty_region(MAX_SIZE));
    // 1 LBA too large
    EXPECT_THROW(gpt.get_empty_region(MAX_SIZE + 1), std::runtime_error);
}

TEST(GptPartitionTableTests, GetEmptyRegionNonEmptyDriveTests) {

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    // for shorter expressions
    static constexpr auto START = TEST_FIRST_USABLE_LBA;

    gpt.save({}, ptr);
    // partitions at LBAs: 0..2, 3..5, 7..9, 12..14, 15..15, 20..25, 32..32
    add_partition(ptr, 0, START + 0, START + 2);
    add_partition(ptr, 1, START + 3, START + 5);
    add_partition(ptr, 2, START + 7, START + 9);
    add_partition(ptr, 3, START + 12, START + 14);
    add_partition(ptr, 4, START + 15, START + 15);
    add_partition(ptr, 5, START + 20, START + 25);
    add_partition(ptr, 6, START + 32, START + 32);
    // invalid entry, should be ignored
    add_partition(ptr, 30, START + TEST_DRIVE_SIZE_LBA, START + TEST_DRIVE_SIZE_LBA);
    update_crcs(ptr);
    gpt.load({}, ptr);

    EXPECT_EQ(START + 6, gpt.get_empty_region(1));
    EXPECT_EQ(START + 10, gpt.get_empty_region(2));
    EXPECT_EQ(START + 16, gpt.get_empty_region(3));
    EXPECT_EQ(START + 16, gpt.get_empty_region(4));
    EXPECT_EQ(START + 26, gpt.get_empty_region(5));
    EXPECT_EQ(START + 26, gpt.get_empty_region(6));
    EXPECT_EQ(START + 33, gpt.get_empty_region(7));
    EXPECT_EQ(START + 33, gpt.get_empty_region(8));
    EXPECT_EQ(START + 33, gpt.get_empty_region(32));
}

TEST(GptPartitionTableTests, GetEmptyRegionReversedNonEmptyDriveTests) {

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    // for shorter expressions
    static constexpr auto START = TEST_FIRST_USABLE_LBA;

    gpt.save({}, ptr);
    // partitions at LBAs: 0..2, 3..5, 7..9, 12..14, 15..15, 20..25, 32..32
    add_partition(ptr, 0, START + 32, START + 32);
    add_partition(ptr, 1, START + 20, START + 25);
    add_partition(ptr, 2, START + 15, START + 15);
    add_partition(ptr, 3, START + 12, START + 14);
    add_partition(ptr, 4, START + 7, START + 9);
    add_partition(ptr, 5, START + 3, START + 5);
    add_partition(ptr, 6, START + 0, START + 2);
    // invalid entry, should be ignored
    add_partition(ptr, 30, START + TEST_DRIVE_SIZE_LBA, START + TEST_DRIVE_SIZE_LBA);
    update_crcs(ptr);
    gpt.load({}, ptr);

    EXPECT_EQ(START + 6, gpt.get_empty_region(1));
    EXPECT_EQ(START + 10, gpt.get_empty_region(2));
    EXPECT_EQ(START + 16, gpt.get_empty_region(3));
    EXPECT_EQ(START + 16, gpt.get_empty_region(4));
    EXPECT_EQ(START + 26, gpt.get_empty_region(5));
    EXPECT_EQ(START + 26, gpt.get_empty_region(6));
    EXPECT_EQ(START + 33, gpt.get_empty_region(7));
    EXPECT_EQ(START + 33, gpt.get_empty_region(8));
    EXPECT_EQ(START + 33, gpt.get_empty_region(32));
}

TEST(GptPartitionTableTests, GetEmptyRegionRandomNonEmptyDriveTests1) {

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    // for shorter expressions
    static constexpr auto START = TEST_FIRST_USABLE_LBA;

    // NON EMPTY GPT TESTS, RANDOM ORDER
    gpt.save({}, ptr);
    // partitions at LBAs: 0..2, 3..5, 7..9, 12..14, 15..15, 20..25, 32..32

    add_partition(ptr, 0, START + 32, START + 32);
    add_partition(ptr, 1, START + 7, START + 9);
    add_partition(ptr, 2, START + 15, START + 15);
    add_partition(ptr, 3, START + 0, START + 2);
    add_partition(ptr, 4, START + 12, START + 14);
    add_partition(ptr, 5, START + 3, START + 5);
    add_partition(ptr, 6, START + 20, START + 25);
    // invalid entry, should be ignored
    add_partition(ptr, 30, START + TEST_DRIVE_SIZE_LBA, START + TEST_DRIVE_SIZE_LBA);
    update_crcs(ptr);
    gpt.load({}, ptr);

    EXPECT_EQ(START + 6, gpt.get_empty_region(1));
    EXPECT_EQ(START + 10, gpt.get_empty_region(2));
    EXPECT_EQ(START + 16, gpt.get_empty_region(3));
    EXPECT_EQ(START + 16, gpt.get_empty_region(4));
    EXPECT_EQ(START + 26, gpt.get_empty_region(5));
    EXPECT_EQ(START + 26, gpt.get_empty_region(6));
    EXPECT_EQ(START + 33, gpt.get_empty_region(7));
    EXPECT_EQ(START + 33, gpt.get_empty_region(8));
    EXPECT_EQ(START + 33, gpt.get_empty_region(32));
}

TEST(GptPartitionTableTests, GetEmptyRegionRandomNonEmptyDriveTests2) {

    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    // for shorter expressions
    static constexpr auto START = TEST_FIRST_USABLE_LBA;

    gpt.save({}, ptr);
    // partitions at LBAs: 0..2, 3..5, 7..9, 12..14, 15..15, 20..25, 32..32

    add_partition(ptr, 4, START + 32, START + 32);
    add_partition(ptr, 10, START + 7, START + 9);
    add_partition(ptr, 8, START + 15, START + 15);
    add_partition(ptr, 2, START + 0, START + 2);
    add_partition(ptr, 6, START + 12, START + 14);
    add_partition(ptr, 0, START + 3, START + 5);
    add_partition(ptr, 12, START + 20, START + 25);
    // invalid entry, should be ignored
    add_partition(ptr, 30, START + TEST_DRIVE_SIZE_LBA, START + TEST_DRIVE_SIZE_LBA);
    update_crcs(ptr);
    gpt.load({}, ptr);

    EXPECT_EQ(START + 6, gpt.get_empty_region(1));
    EXPECT_EQ(START + 10, gpt.get_empty_region(2));
    EXPECT_EQ(START + 16, gpt.get_empty_region(3));
    EXPECT_EQ(START + 16, gpt.get_empty_region(4));
    EXPECT_EQ(START + 26, gpt.get_empty_region(5));
    EXPECT_EQ(START + 26, gpt.get_empty_region(6));
    EXPECT_EQ(START + 33, gpt.get_empty_region(7));
    EXPECT_EQ(START + 33, gpt.get_empty_region(8));
    EXPECT_EQ(START + 33, gpt.get_empty_region(32));
}

TEST(GptPartitionTableTests, GetUsableBlocks) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data.get_raw_vector().resize(TEST_DRIVE_SIZE_LBA * LBA_SIZE);

    GptPartitionTable gpt(LBA_SIZE);
    gpt.reset(TEST_DRIVE_SIZE_LBA);

    EXPECT_EQ(TEST_LAST_USABLE_LBA - TEST_FIRST_USABLE_LBA + 1, gpt.get_max_usable_blocks());
}

}
