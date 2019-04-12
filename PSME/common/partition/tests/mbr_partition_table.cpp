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

#include "partition/mbr_partition_table.hpp"
#include "mock_drive_interface.hpp"

#include "gtest/gtest.h"

using namespace partition;
using namespace utils;

namespace {

static constexpr unsigned EXPECTED_MBR_SIZE = 512;
static constexpr unsigned EXPECTED_MBR_BOOTSTRAP_SIZE = 446;
static constexpr unsigned EXPECTED_MBR_PARTITION_SIZE = 16;
static constexpr unsigned EXPECTED_MBR_PARTITION_ENTRY_COUNT = 4;
static constexpr unsigned EXPECTED_MBR_PARTITION_ENTRY_OFFSET[] = {446, 462, 478, 494};
static constexpr unsigned EXPECTED_MBR_BOOT_SIGNATURE_OFFSET = 510;
static constexpr uint16_t EXPECTED_MBR_BOOT_SIGNATURE = 0xAA55;
static constexpr uint8_t EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE = 0xEE;
static constexpr uint8_t TEST_MBR_RAW[EXPECTED_MBR_SIZE] = {
/* 0x0000 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0010 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0020 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0030 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0040 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0050 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0060 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0070 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0080 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0090 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00A0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00B0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00C0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00D0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00E0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x00F0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0100 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0110 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0120 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0130 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0140 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0150 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0160 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0170 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0180 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x0190 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x01A0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
/* 0x01B0 */ 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x25, 0x35,
/* 0x01C0 */ 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5, 0xC5, 0xD5, 0xE5, 0xF5, 0x05, 0x15, 0x26, 0x36,
/* 0x01D0 */ 0x46, 0x56, 0x66, 0x76, 0x86, 0x96, 0xA6, 0xB6, 0xC6, 0xD6, 0xE6, 0xF6, 0x06, 0x16, 0x27, 0x37,
/* 0x01E0 */ 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7, 0x07, 0x17, 0x28, 0x38,
/* 0x01F0 */ 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xA8, 0xB8, 0xC8, 0xD8, 0xE8, 0xF8, 0x08, 0x18, 0x39, 0x41
};

/*
 * This is an expected mbr protective partition entry for a drive with size 0x2E9390AF
 * Additional numbers are absolute adresses of the first and last sectors (exspressed in CHS),
 * MbrProtective usually puts here maximal possible values
 */
static constexpr uint64_t TEST_SIZE = 0x2E9390AF;
static constexpr uint8_t EXPECTED_MBR_PROTECTIVE_ENTRY[EXPECTED_MBR_PARTITION_SIZE] = {
    0x00, 0x00, 0x01, 0x00, 0xEE, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xAF, 0x90, 0x93, 0x2E
};

/* Used to test mbr_to_bb and bb_to_mbr protected MbrPartitionTable methods */
class GetProtectedMbrMembers final : public MbrPartitionTable {
public:
    virtual ~GetProtectedMbrMembers() {}

    using MbrPartitionData = MbrPartitionTable::MbrPartitionData;
    using MbrData = MbrPartitionTable::MbrData;

    static MbrData do_bb_to_mbr(const utils::ByteBuffer& bb) {
        return MbrPartitionTable::bb_to_mbr(bb);
    }
    static utils::ByteBuffer do_mbr_to_bb(const MbrData& mbr) {
        return MbrPartitionTable::mbr_to_bb(mbr);
    }
};

}

namespace testing {

TEST(MbrPartitionTableTests, ByteBufferToMbrConversion) {
    ByteBuffer mock_mbr = std::vector<uint8_t>(std::begin(TEST_MBR_RAW), std::end(TEST_MBR_RAW));
    ASSERT_EQ(mock_mbr.size(), EXPECTED_MBR_SIZE);
    auto mbr = GetProtectedMbrMembers::do_bb_to_mbr(mock_mbr);

    for (unsigned i = 0; i < EXPECTED_MBR_BOOTSTRAP_SIZE; ++i) {
        EXPECT_EQ(mbr.bootstrap[i], 0x11);
    }
    for (unsigned i = 0; i < EXPECTED_MBR_PARTITION_ENTRY_COUNT; ++i) {
        EXPECT_EQ(mbr.partition_data[i].status, 0x25 + i);
        EXPECT_EQ(mbr.partition_data[i].first_sector_chs[0], 0x35 + i);
        EXPECT_EQ(mbr.partition_data[i].first_sector_chs[1], 0x45 + i);
        EXPECT_EQ(mbr.partition_data[i].first_sector_chs[2], 0x55 + i);
        EXPECT_EQ(mbr.partition_data[i].partition_type, 0x65 + i);
        EXPECT_EQ(mbr.partition_data[i].last_sector_chs[0], 0x75 + i);
        EXPECT_EQ(mbr.partition_data[i].last_sector_chs[1], 0x85 + i);
        EXPECT_EQ(mbr.partition_data[i].last_sector_chs[2], 0x95 + i);
        EXPECT_EQ(mbr.partition_data[i].first_sector_lba, 0xD5C5B5A5 + i * 0x01010101);
        EXPECT_EQ(mbr.partition_data[i].num_of_sectors, 0x1505F5E5 + i * 0x01010101);
    }
    EXPECT_EQ(mbr.boot_signature, 0x4139);
}

TEST(MbrPartitionTableTests, MbrToByteBufferConversion) {
    GetProtectedMbrMembers::MbrData mbr{};
    for (unsigned i = 0; i < EXPECTED_MBR_BOOTSTRAP_SIZE; ++i) {
        mbr.bootstrap[i] = 0x11;
    }
    for (unsigned i = 0; i < EXPECTED_MBR_PARTITION_ENTRY_COUNT; ++i) {
        mbr.partition_data[i].status = uint8_t(0x25 + i);
        mbr.partition_data[i].first_sector_chs[0] = uint8_t(0x35 + i);
        mbr.partition_data[i].first_sector_chs[1] = uint8_t(0x45 + i);
        mbr.partition_data[i].first_sector_chs[2] = uint8_t(0x55 + i);
        mbr.partition_data[i].partition_type = uint8_t(0x65 + i);
        mbr.partition_data[i].last_sector_chs[0] = uint8_t(0x75 + i);
        mbr.partition_data[i].last_sector_chs[1] = uint8_t(0x85 + i);
        mbr.partition_data[i].last_sector_chs[2] = uint8_t(0x95 + i);
        mbr.partition_data[i].first_sector_lba = uint32_t(0xD5C5B5A5 + i * 0x01010101);
        mbr.partition_data[i].num_of_sectors = uint32_t(0x1505F5E5 + i * 0x01010101);
    }
    mbr.boot_signature = uint16_t(0x4139);

    auto buf = GetProtectedMbrMembers::do_mbr_to_bb(mbr);
    ASSERT_EQ(buf.size(), EXPECTED_MBR_SIZE);

    for (unsigned i = 0; i < EXPECTED_MBR_SIZE; ++i) {
        EXPECT_EQ(buf[i], TEST_MBR_RAW[i]);
    }
}

TEST(MbrPartitionTableTests, PartitionsAreReadCorrectly) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data = ByteBuffer(EXPECTED_MBR_SIZE);
    MbrPartitionTable pt{};

    EXPECT_THROW(pt.load({}, ptr), std::runtime_error);
    ASSERT_FALSE(pt.is_valid());

    ptr->mocked_data.set_le16(EXPECTED_MBR_BOOT_SIGNATURE_OFFSET, 0x1234);
    EXPECT_THROW(pt.load({}, ptr), std::runtime_error);
    ASSERT_FALSE(pt.is_valid());

    ptr->mocked_data.set_le16(EXPECTED_MBR_BOOT_SIGNATURE_OFFSET, EXPECTED_MBR_BOOT_SIGNATURE);
    pt.load({}, ptr);
    ASSERT_TRUE(pt.is_valid());
    ASSERT_TRUE(pt.get_partition_ids().empty());
    ASSERT_EQ(pt.get_available_partition_id(), 0);

    // Two partitions -> Unknown in slot 0 and 3 and Protective MBR in slot 2
    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[0] + 4] = 0x10;
    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[1] + 4] = 0x00;
    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[2] + 4] = EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE;
    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[3] + 4] = 0x20;
    pt.load({}, ptr);
    ASSERT_TRUE(pt.is_valid());
    ASSERT_TRUE(pt.get_partition_ids() == std::vector<unsigned>({0, 2, 3}));
    ASSERT_EQ(pt.get_available_partition_id(), 1);
    auto partition = pt.get_partition(0);
    ASSERT_EQ(partition.type, PartitionType::UNKNOWN);
    ASSERT_THROW(pt.get_partition(1), std::runtime_error);
    partition = pt.get_partition(2);
    ASSERT_EQ(partition.type, PartitionType::MBR_PROTECTIVE);
    partition = pt.get_partition(3);
    ASSERT_EQ(partition.type, PartitionType::UNKNOWN);

    // reset resets to an empty, but valid partition table
    pt.reset(100);
    ASSERT_TRUE(pt.is_valid());
    ASSERT_TRUE(pt.get_partition_ids().empty());
    ASSERT_EQ(pt.get_available_partition_id(), 0);
}

TEST(MbrPartitionTableTests, PartitionDataIsReadCorrectly) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data = ByteBuffer(EXPECTED_MBR_SIZE);
    ptr->mocked_data.set_le16(EXPECTED_MBR_BOOT_SIGNATURE_OFFSET, EXPECTED_MBR_BOOT_SIGNATURE);

    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[0] + 4] = EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE;
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[0] + 8, 10);
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[0] + 12, 10);

    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[1] + 4] = EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE;
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[1] + 8, 30);
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[1] + 12, 1);

    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[2] + 4] = EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE;
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[2] + 8, 0);
    ptr->mocked_data.set_le32(EXPECTED_MBR_PARTITION_ENTRY_OFFSET[2] + 12, 101);

    ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[3] + 4] = EXPECTED_MBR_PARTITION_TYPE_PROTECTIVE;

    MbrPartitionTable pt{};
    pt.load({}, ptr);
    ASSERT_TRUE(pt.is_valid());
    ASSERT_TRUE(pt.get_partition_ids() == std::vector<unsigned>({0, 1, 2, 3}));
    try {
        pt.get_available_partition_id();
        FAIL() << "Exception should be thrown";
    }
    catch (const MbrPartitionTable::IdUnavailable&) {
        // expected exception
    }
    catch (...) {
        FAIL() << "Invalid exception thrown";
    }

    auto partition = pt.get_partition(0);
    ASSERT_EQ(partition.type, PartitionType::MBR_PROTECTIVE);
    ASSERT_EQ(partition.first_lba, 10);
    ASSERT_EQ(partition.size_lba, 10);
    ASSERT_TRUE(partition.name.empty());

    partition = pt.get_partition(1);
    ASSERT_EQ(partition.type, PartitionType::MBR_PROTECTIVE);
    ASSERT_EQ(partition.first_lba, 30);
    ASSERT_EQ(partition.size_lba, 1);
    ASSERT_TRUE(partition.name.empty());

    partition = pt.get_partition(2);
    ASSERT_EQ(partition.type, PartitionType::MBR_PROTECTIVE);
    ASSERT_EQ(partition.first_lba, 0);
    ASSERT_EQ(partition.size_lba, 101);
    ASSERT_TRUE(partition.name.empty());

    partition = pt.get_partition(3);
    ASSERT_EQ(partition.type, PartitionType::MBR_PROTECTIVE);
    ASSERT_EQ(partition.first_lba, 0);
    ASSERT_EQ(partition.size_lba, 0);
    ASSERT_TRUE(partition.name.empty());
}


TEST(MbrPartitionTableTests, ClearCreatesAnEmptyPartitionTable) {
    MbrPartitionTable pt{};
    pt.reset(100);

    ASSERT_TRUE(pt.is_valid());
    ASSERT_TRUE(pt.get_partition_ids() == std::vector<unsigned>({}));
    ASSERT_EQ(pt.get_available_partition_id(), 0);
}

TEST(MbrPartitionTableTests, RemovePartitions) {
    MbrPartitionTable pt{};
    pt.reset(100);

    EXPECT_THROW(pt.get_partition(1), std::runtime_error);

    PartitionData partition{};
    partition.type = PartitionType::MBR_PROTECTIVE;
    partition.size_lba = TEST_SIZE;
    pt.set_partition(0, partition);
    pt.update();

    EXPECT_TRUE(pt.is_valid());
    EXPECT_TRUE(pt.get_partition_ids() == std::vector<unsigned>({0}));
    pt.get_partition(0);

    pt.remove_partition(0);
    pt.update();
    EXPECT_TRUE(pt.is_valid());
    EXPECT_TRUE(pt.get_partition_ids() == std::vector<unsigned>({}));

    EXPECT_THROW(pt.remove_partition(100), std::runtime_error);
}

TEST(MbrPartitionTableTests, InvalidIdHandling) {
    MbrPartitionTable pt{};
    try {
        pt.get_partition(20);
        FAIL() << "Invalid Id was passed, but no exception was generated";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

TEST(MbrPartitionTableTests, NotSupportedPartitionsCannotBeCreated) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data = ByteBuffer(EXPECTED_MBR_SIZE);

    MbrPartitionTable pt{};
    pt.reset(100);

    try {
        pt.set_partition(1, PartitionData{});
        FAIL() << "Invalid Id was passed, but no exception was generated";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }

    try {
        PartitionData p{};
        p.type = PartitionType::UNKNOWN;
        pt.set_partition(0, p);
        FAIL() << "Invalid Id was passed, but no exception was generated";
    }
    catch (const std::runtime_error&) {
        // expected
    }
    catch (...) {
        FAIL() << "Invalid exception type";
    }
}

TEST(MbrPartitionTableTests, PartitionsAreCreatedCorrectly) {
    auto ptr = std::make_shared<MockDriveInterface>();
    ptr->mocked_data = ByteBuffer(EXPECTED_MBR_SIZE);

    MbrPartitionTable pt{};
    pt.reset(100);

    PartitionData partition{};
    partition.type = PartitionType::MBR_PROTECTIVE;
    partition.size_lba = TEST_SIZE;
    pt.set_partition(0, partition);
    ASSERT_FALSE(pt.is_valid());
    pt.update();
    ASSERT_TRUE(pt.is_valid());
    pt.save({}, ptr);

    // these should be empty
    for (unsigned i = 0; i < EXPECTED_MBR_BOOTSTRAP_SIZE; ++i) {
        EXPECT_EQ(ptr->mocked_data[i], 0x00);
    }
    for (unsigned i = 0; i < 3 * EXPECTED_MBR_PARTITION_SIZE; ++i) {
        EXPECT_EQ(ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[1] + i], 0x00);
    }

    // boot signature
    EXPECT_EQ(ptr->mocked_data.get_le16(EXPECTED_MBR_BOOT_SIGNATURE_OFFSET), EXPECTED_MBR_BOOT_SIGNATURE);

    // correct partition entry for the mbr protective partition
    for (unsigned i = 0; i < EXPECTED_MBR_PARTITION_SIZE; ++i) {
        EXPECT_EQ(ptr->mocked_data[EXPECTED_MBR_PARTITION_ENTRY_OFFSET[0] + i], EXPECTED_MBR_PROTECTIVE_ENTRY[i]);
    }

}

TEST(MbrPartitionTableTests, GetEmptyRegion) {
    MbrPartitionTable pt{};
    pt.reset(100);
    // This method is currently not implemented
    // If implementation is added - add unit tests
    EXPECT_THROW(pt.get_empty_region(1), std::runtime_error);
}

TEST(MbrPartitionTableTests, GetUsableBlocks) {
    MbrPartitionTable pt{};
    pt.reset(100);
    // This method is currently not implemented
    // If implementation is added - add unit tests
    EXPECT_THROW(pt.get_max_usable_blocks(), std::runtime_error);
}

}
