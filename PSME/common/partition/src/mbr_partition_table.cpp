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
 * @file partition/mbr_partition_table.cpp
 */

#include "partition/mbr_partition_table.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <safe-string/safe_lib.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <sstream>

using namespace partition;
using namespace utils;

constexpr unsigned MbrPartitionTable::MBR_PARTITION_ENTRY_OFFSET[];
constexpr uint8_t MbrPartitionTable::MBR_PROTECTIVE_FIRST_SECTOR_CHS[];
constexpr uint8_t MbrPartitionTable::MBR_PROTECTIVE_LAST_SECTOR_CHS[];

MbrPartitionTable::~MbrPartitionTable() {
}

MbrPartitionTable::MbrData MbrPartitionTable::bb_to_mbr(const ByteBuffer& bb) {
    MbrPartitionTable::MbrData mbr{};
    memcpy_s(reinterpret_cast<void*>(&mbr.bootstrap), MBR_BOOTSTRAP_SIZE,
             reinterpret_cast<const void*>(bb.data()), MBR_BOOTSTRAP_SIZE);
    mbr.boot_signature = bb.get_le16(MBR_BOOT_SIGNATURE_OFFSET);
    for (unsigned i = 0; i < MBR_PARTITION_ENTRY_COUNT; ++i) {
        auto offset = MBR_PARTITION_ENTRY_OFFSET[i];
        mbr.partition_data[i].status = bb[offset + 0];
        mbr.partition_data[i].first_sector_chs[0] = bb[offset + 1];
        mbr.partition_data[i].first_sector_chs[1] = bb[offset + 2];
        mbr.partition_data[i].first_sector_chs[2] = bb[offset + 3];
        mbr.partition_data[i].partition_type = bb[offset + 4];
        mbr.partition_data[i].last_sector_chs[0] = bb[offset + 5];
        mbr.partition_data[i].last_sector_chs[1] = bb[offset + 6];
        mbr.partition_data[i].last_sector_chs[2] = bb[offset + 7];
        mbr.partition_data[i].first_sector_lba = bb.get_le32(offset + 8);
        mbr.partition_data[i].num_of_sectors = bb.get_le32(offset + 12);
    }
    return mbr;
}

ByteBuffer MbrPartitionTable::mbr_to_bb(const MbrPartitionTable::MbrData& mbr) {
    ByteBuffer bb(MBR_DATA_SIZE);
    memcpy_s(reinterpret_cast<void*>(bb.data()), MBR_BOOTSTRAP_SIZE,
             reinterpret_cast<const void*>(&mbr.bootstrap), MBR_BOOTSTRAP_SIZE);
    bb.set_le16(MBR_BOOT_SIGNATURE_OFFSET, mbr.boot_signature);
    for (unsigned i = 0; i < MBR_PARTITION_ENTRY_COUNT; ++i) {
        auto offset = MBR_PARTITION_ENTRY_OFFSET[i];
        bb[offset + 0] = mbr.partition_data[i].status;
        bb[offset + 1] = mbr.partition_data[i].first_sector_chs[0];
        bb[offset + 2] = mbr.partition_data[i].first_sector_chs[1];
        bb[offset + 3] = mbr.partition_data[i].first_sector_chs[2];
        bb[offset + 4] = mbr.partition_data[i].partition_type;
        bb[offset + 5] = mbr.partition_data[i].last_sector_chs[0];
        bb[offset + 6] = mbr.partition_data[i].last_sector_chs[1];
        bb[offset + 7] = mbr.partition_data[i].last_sector_chs[2];
        bb.set_le32(offset + 8, mbr.partition_data[i].first_sector_lba);
        bb.set_le32(offset + 12, mbr.partition_data[i].num_of_sectors);
    }
    return bb;
}

void MbrPartitionTable::load(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) {
    m_is_valid = false;
    m_mbr = bb_to_mbr(interface->read(target, MBR_OFFSET, MBR_DATA_SIZE));

    // this is the only validity check we can do
    if (m_mbr.boot_signature == MBR_BOOT_SIGNATURE) {
        m_is_valid = true;
        return;
    }
    throw std::runtime_error("Unable to read MBR partition table: signature not present!");
}

void MbrPartitionTable::save(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) const {
    if (!m_is_valid) {
        throw std::logic_error("MBR partition table is not valid and will not be written!");
    }
    interface->write(target, MBR_OFFSET, mbr_to_bb(m_mbr));
}

std::vector<unsigned> MbrPartitionTable::get_partition_ids() const {
    std::vector<unsigned> ids{};
    for (unsigned i = 0; i < MBR_PARTITION_ENTRY_COUNT; ++i) {
        if (m_mbr.partition_data[i].partition_type != MBR_PARTITION_TYPE_NONE) {
            ids.push_back(i);
        }
    }
    return ids;
}

unsigned MbrPartitionTable::get_available_partition_id() const {
    for (unsigned i = 0; i < MBR_PARTITION_ENTRY_COUNT; ++i) {
        if (m_mbr.partition_data[i].partition_type == MBR_PARTITION_TYPE_NONE) {
            return i;
        }
    }
    throw IdUnavailable{};
}

PartitionData MbrPartitionTable::get_partition(unsigned id) const {
    if (id >= MBR_PARTITION_ENTRY_COUNT) {
        std::stringstream str{};
        str << "Invalid partition id: " << id << ", max is " << MBR_PARTITION_ENTRY_COUNT;
        throw std::runtime_error(str.str());
    }

    PartitionData pd{};
    pd.first_lba = m_mbr.partition_data[id].first_sector_lba;
    pd.size_lba = m_mbr.partition_data[id].num_of_sectors;
    if (m_mbr.partition_data[id].partition_type == MBR_PARTITION_TYPE_NONE) {
        throw std::runtime_error(std::string{"No partition on slot "} + std::to_string(id));
    }
    else if (m_mbr.partition_data[id].partition_type == MBR_PARTITION_TYPE_PROTECTIVE) {
        pd.type = PartitionType::MBR_PROTECTIVE;
    }
    else {
        // we do not really care about other types
        pd.type = PartitionType::UNKNOWN;
    }

    // uuid/name not available in MBR

    return pd;
}

void MbrPartitionTable::set_partition(unsigned id, const PartitionData& pd) {
    if (id >= MBR_PARTITION_ENTRY_COUNT) {
        std::stringstream str{};
        str << "Invalid partition id: " << id << ", max is " << MBR_PARTITION_ENTRY_COUNT;
        throw std::runtime_error(str.str());
    }
    if (pd.type != PartitionType::MBR_PROTECTIVE || id != 0) {
        throw std::runtime_error("Not implemented, only MBR Protective partitions are supported!");
    }

    m_is_valid = false;
    m_mbr.partition_data[id].partition_type = MBR_PARTITION_TYPE_PROTECTIVE;

    for (unsigned i = 0; i < 3; ++i) {
        m_mbr.partition_data[id].first_sector_chs[i] = MBR_PROTECTIVE_FIRST_SECTOR_CHS[i];
        m_mbr.partition_data[id].last_sector_chs[i] = MBR_PROTECTIVE_LAST_SECTOR_CHS[i];
    }

    static constexpr uint32_t MAX_U32 = 0xFFFFFFFF;
    m_mbr.partition_data[id].first_sector_lba = MBR_PROTECTIVE_FIRST_AVAILABLE_LBA;
    m_mbr.partition_data[id].num_of_sectors = (pd.size_lba > MAX_U32 ? MAX_U32 : uint32_t(pd.size_lba));
}

void MbrPartitionTable::remove_partition(unsigned id) {
    if (id >= MBR_PARTITION_ENTRY_COUNT) {
        std::stringstream str{};
        str << "Invalid partition id: " << id << ", max is " << MBR_PARTITION_ENTRY_COUNT;
        throw std::runtime_error(str.str());
    }
    m_is_valid = false;

    m_mbr.partition_data[id] = MbrPartitionData{};
}

uint64_t MbrPartitionTable::get_empty_region(uint64_t) const {
    throw std::runtime_error("Not implemented");
}

uint64_t MbrPartitionTable::get_max_usable_blocks() const {
    throw std::runtime_error("Not implemented");
}

void MbrPartitionTable::reset(uint64_t /*drive_size_lba*/) {
    m_mbr = MbrData{};
    update();
}

void MbrPartitionTable::update() {
    m_mbr.boot_signature = MBR_BOOT_SIGNATURE;
    m_is_valid = true;
}
