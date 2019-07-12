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

#include "partition/gpt_partition_table.hpp"
#include "crc/crc32.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#include <safe-string/safe_lib.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wextra-semi"
#endif

#include "uuid/uuid.hpp"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <sstream>
#include <iomanip>
#include <regex>

using namespace partition;
using namespace utils;

namespace {

uint64_t integer_divide_and_ceil(uint64_t a, uint64_t b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero!");
    }
    // this is a trick to make integer division with ceil operation without using floating point numbers
    return (a + b - 1) / b;
}

}

constexpr unsigned GptPartitionTable::GPT_PARTITION_NAME_SIZE;
constexpr unsigned GptPartitionTable::GPT_GUID_SIZE;
constexpr char GptPartitionTable::GPT_EMPTY_GUID[];
constexpr char GptPartitionTable::GPT_PARTITION_TYPE_UUID_LINUX[];

GptPartitionTable::~GptPartitionTable() {
}

GptPartitionTable::GptHeaderData GptPartitionTable::bb_to_gpt_header(const ByteBuffer& bb) {
    GptHeaderData gpt{};
    gpt.signature = bb.get_le64(0);
    gpt.revision = bb.get_le32(8);
    gpt.header_size = bb.get_le32(12);
    gpt.header_crc = bb.get_le32(16);
    // omitted reserved 4 bytes
    gpt.current_lba = bb.get_le64(24);
    gpt.backup_lba = bb.get_le64(32);
    gpt.first_usable_lba = bb.get_le64(40);
    gpt.last_usable_lba = bb.get_le64(48);
    gpt.disk_guid = get_guid(bb, 56);
    gpt.partition_table_lba = bb.get_le64(72);
    gpt.num_of_partitions = bb.get_le32(80);
    gpt.partition_entry_size = bb.get_le32(84);
    gpt.partition_table_crc = bb.get_le32(88);
    return gpt;
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunsafe-loop-optimizations"
#endif

GptPartitionTable::GptTableData GptPartitionTable::bb_to_gpt_table(const ByteBuffer& bb) {
    GptTableData gpt{{}};
    for (unsigned i = 0; i < GPT_PARTITION_COUNT; ++i) {
        auto offset = i * GPT_PARTITION_DATA_SIZE;
        gpt[i].type_guid = get_guid(bb, offset);
        gpt[i].guid = get_guid(bb, offset + 16);
        gpt[i].first_lba = bb.get_le64(offset + 32);
        gpt[i].last_lba = bb.get_le64(offset + 40);
        gpt[i].flags = bb.get_le64(offset + 48);
        gpt[i].name = get_name(bb, offset + 56);
    }
    return gpt;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

ByteBuffer GptPartitionTable::gpt_header_to_bb(const GptPartitionTable::GptHeaderData& gpt) {
    ByteBuffer bb(GPT_HEADER_DATA_SIZE);
    bb.set_le64(0, gpt.signature);
    bb.set_le32(8, gpt.revision);
    bb.set_le32(12, gpt.header_size);
    bb.set_le32(16, gpt.header_crc);
    bb.set_le64(24, gpt.current_lba);
    bb.set_le64(32, gpt.backup_lba);
    bb.set_le64(40, gpt.first_usable_lba);
    bb.set_le64(48, gpt.last_usable_lba);
    set_guid(bb, 56, gpt.disk_guid);
    bb.set_le64(72, gpt.partition_table_lba);
    bb.set_le32(80, gpt.num_of_partitions);
    bb.set_le32(84, gpt.partition_entry_size);
    bb.set_le32(88, gpt.partition_table_crc);
    return bb;
}

ByteBuffer GptPartitionTable::gpt_table_to_bb(const GptPartitionTable::GptTableData& gpt) {
    ByteBuffer bb(GPT_PARTITION_COUNT * GPT_PARTITION_DATA_SIZE);
    for (unsigned i = 0; i < GPT_PARTITION_COUNT; ++i) {
        auto offset = i * GPT_PARTITION_DATA_SIZE;
        set_guid(bb, offset, gpt[i].type_guid);
        set_guid(bb, offset + 16, gpt[i].guid);
        bb.set_le64(offset + 32, gpt[i].first_lba);
        bb.set_le64(offset + 40, gpt[i].last_lba);
        bb.set_le64(offset + 48, gpt[i].flags);
        set_name(bb, offset + 56, gpt[i].name);
    }
    return bb;
}

std::string GptPartitionTable::get_guid(const utils::ByteBuffer& bb, uint64_t _offset) {
    std::stringstream str{};
    std::size_t offset = std::size_t(_offset);
    str << std::hex << std::uppercase << std::setfill('0');
    str << std::setw(8) << bb.get_le32(offset + 0u) << "-"
        << std::setw(4) << bb.get_le16(offset + 4u) << "-"
        << std::setw(4) << bb.get_le16(offset + 6u) << "-"
        << std::setw(4) << bb.get_be16(offset + 8u) << "-"
        << std::setw(12) << bb.get_be48(offset + 10u);
    return str.str();
}

std::string GptPartitionTable::get_name(const utils::ByteBuffer& bb, uint64_t _offset) {
    std::stringstream str{};
    std::size_t offset = std::size_t(_offset);
    for (unsigned i = 0; i < GPT_PARTITION_NAME_SIZE / 2; ++i) {
        // two bytes per one char
        uint8_t ch = uint8_t(bb.get_le16(offset + i * 2));
        if (ch != 0) {
            str << ch;
        }
    }
    return str.str();
}

void GptPartitionTable::set_guid(utils::ByteBuffer& bb, uint64_t _offset, const std::string& guid) {
    std::size_t offset = std::size_t(_offset);
    static constexpr unsigned EXPECTED_MATCHES = 6;
    static constexpr int HEX_BASE = 16;
    std::regex expr("([0-9a-fA-F]{8})-([0-9a-fA-F]{4})-([0-9a-fA-F]{4})-([0-9a-fA-F]{4})-([0-9a-fA-F]{12})");
    std::smatch match{};
    if (!std::regex_match(guid, match, expr) || match.size() != EXPECTED_MATCHES) {
        throw std::runtime_error(std::string{"Invalid GUID format: "} + guid);
    }

    bb.set_le32(offset + 0, uint32_t(std::stoul(match[1].str(), nullptr, HEX_BASE)));
    bb.set_le16(offset + 4, uint16_t(std::stoul(match[2].str(), nullptr, HEX_BASE)));
    bb.set_le16(offset + 6, uint16_t(std::stoul(match[3].str(), nullptr, HEX_BASE)));
    bb.set_be16(offset + 8, uint16_t(std::stoul(match[4].str(), nullptr, HEX_BASE)));
    bb.set_be48(offset + 10, std::stoull(match[5].str(), nullptr, HEX_BASE));
}

void GptPartitionTable::set_name(utils::ByteBuffer& bb, uint64_t _offset, const std::string& name) {
    std::size_t offset = std::size_t(_offset);
    auto chars_to_write = std::min(unsigned(name.size()), GPT_PARTITION_NAME_SIZE / 2);
    for (unsigned i = 0; i < chars_to_write; ++i) {
        bb.set_le16(offset + i * 2, uint16_t(name[i]));
    }
    // fill remaining bytes with zeros
    for (unsigned i = chars_to_write; i < GPT_PARTITION_NAME_SIZE / 2; ++i) {
        bb.set_le16(offset + i * 2, 0);
    }
}

void GptPartitionTable::load_gpt(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface,
    uint64_t header_lba, bool is_primary) {

    // read header, check if GPT signature is present
    m_gpt_header = bb_to_gpt_header(interface->read(target, header_lba * m_bytes_per_lba, GPT_HEADER_DATA_SIZE));
    if (m_gpt_header.signature != GPT_SIGNATURE) {
        throw std::runtime_error("No GPT signature");
    }

    uint64_t table_lba = (is_primary ? m_gpt_header.partition_table_lba : m_gpt_header.last_usable_lba + 1);
    // read partition table
    m_gpt_table = bb_to_gpt_table(interface->read(target, table_lba * m_bytes_per_lba,
        GPT_PARTITION_DATA_SIZE * GPT_PARTITION_COUNT));

    // check CRCs, if true, this is a valid GPT table
    if (!check_crc()) {
        throw std::runtime_error("CRC check fail");
    }
}

void GptPartitionTable::load_mbr(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) {

    // fail if mbr is not present
    m_mbr.load(target, interface);
    if (!m_mbr.is_valid()) {
        throw std::runtime_error("Unable to read GPT partition table: MBR subpart is not valid!");
    }

    // fail if MBR protective partition is not present
    auto mbr_partitions = m_mbr.get_partition_ids();
    if (mbr_partitions.size() != 1) {
        throw std::runtime_error("Unable to read GPT partition table: no MBR partitions present");
    }
    if (mbr_partitions[0] != 0) {
        throw std::runtime_error("Unable to read GPT partition table: no partition on MBR slot 0");
    }
    if (m_mbr.get_partition(mbr_partitions[0]).type != PartitionType::MBR_PROTECTIVE) {
        throw std::runtime_error("Unable to read GPT partition table: no MBR protective partition");
    }
}

void GptPartitionTable::load(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) {

    m_is_valid = false;
    load_mbr(target, interface);

    // check GPT, if primary table is invalid, try with the backup table
    // note that location of the backup table (m_gpt_header.backup_lba) is read by the first load_gpt call
    try {
        load_gpt(target, interface, GPT_HEADER_LOCATION, true);
    }
    catch (const std::exception& e) {
        try {
            load_gpt(target, interface, m_gpt_header.backup_lba, false);
        }
        catch (const std::exception& ee) {
            throw std::runtime_error(std::string{"Unable to read GPT partition table: primary table is invalid ("}
                + e.what() + "), reading backup failed (" + ee.what() + ")");
        }
    }
    m_is_valid = true;
}

std::vector<unsigned> GptPartitionTable::get_partition_ids() const {
    std::vector<unsigned> ids{};
    for (unsigned i = 0; i < GPT_PARTITION_COUNT; ++i) {
        if (m_gpt_table[i].type_guid != GPT_EMPTY_GUID) {
            ids.push_back(i);
        }
    }
    return ids;
}

unsigned GptPartitionTable::get_available_partition_id() const {
    for (unsigned i = 0; i < GPT_PARTITION_COUNT; ++i) {
        if (m_gpt_table[i].type_guid == GPT_EMPTY_GUID) {
            return i;
        }
    }
    throw IdUnavailable{};
}

PartitionData GptPartitionTable::get_partition(unsigned id) const {
    if (id >= GPT_PARTITION_COUNT) {
        std::stringstream str{};
        str << "Invalid partition id: " << id << ", max is " << GPT_PARTITION_COUNT;
        throw std::runtime_error(str.str());
    }

    PartitionData pd{};

    if (m_gpt_table[id].type_guid == GPT_EMPTY_GUID) {
        throw std::runtime_error(std::string{"No partition on slot "} + std::to_string(id));
    }
    else if (m_gpt_table[id].type_guid == GPT_PARTITION_TYPE_UUID_LINUX) {
        pd.type = PartitionType::LINUX;
    }
    else {
        // we do not really care about other types
        pd.type = PartitionType::UNKNOWN;
    }

    pd.first_lba = m_gpt_table[id].first_lba;
    if (m_gpt_table[id].last_lba < m_gpt_table[id].first_lba) {
        throw std::runtime_error("Invalid partition data: last LBA < first LBA");
    }
    pd.size_lba = m_gpt_table[id].last_lba - m_gpt_table[id].first_lba + 1;

    pd.name = m_gpt_table[id].name;
    pd.uuid = m_gpt_table[id].guid;

    return pd;
}

void GptPartitionTable::save(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) const {

    if (!m_is_valid) {
        throw std::runtime_error("Trying to save invalid GPT partition table, operation aborted!");
    }

    // start with saving mbr
    m_mbr.save(target, interface);

    // get offsets
    auto header_location_lba = m_gpt_header.current_lba;
    auto header_backup_location_lba = m_gpt_header.backup_lba;
    auto table_location_lba = m_gpt_header.current_lba + GPT_HEADER_SIZE_LBA;
    auto table_backup_location_lba = m_gpt_header.last_usable_lba + 1; // backup table starts just after the last usable lba

    // original copy
    interface->write(target, header_location_lba * m_bytes_per_lba, gpt_header_to_bb(m_gpt_header));
    interface->write(target, table_location_lba * m_bytes_per_lba, gpt_table_to_bb(m_gpt_table));

    // create valid backup header
    auto backup_header = m_gpt_header;
    std::swap(backup_header.current_lba, backup_header.backup_lba);
    backup_header.partition_table_lba = table_backup_location_lba;
    backup_header.header_crc = 0;
    auto backup_buffer = gpt_header_to_bb(backup_header);
    backup_header.header_crc = crc::Crc32(backup_buffer.data(), GPT_HEADER_DATA_SIZE);

    // backup copy
    interface->write(target, header_backup_location_lba * m_bytes_per_lba, gpt_header_to_bb(backup_header));
    interface->write(target, table_backup_location_lba * m_bytes_per_lba, gpt_table_to_bb(m_gpt_table));
}

GptPartitionTable::GptPartitionData GptPartitionTable::get_linux_partition_entry(const PartitionData& pd) {

    GptPartitionData entry{};
    entry.type_guid = GPT_PARTITION_TYPE_UUID_LINUX;
    entry.guid = make_v1_uuid();

    entry.name = pd.name;
    entry.first_lba = pd.first_lba;
    entry.last_lba = pd.first_lba + pd.size_lba - 1;
    if (entry.last_lba < entry.first_lba) {
        entry.last_lba = entry.first_lba;
    }
    return entry;
}

void GptPartitionTable::set_partition(unsigned id, const PartitionData& pd) {
    // validate params
    if (id > GPT_PARTITION_COUNT) {
        throw std::runtime_error("Unsupported (too large) partition ID!");
    }
    if (pd.name.size() > GPT_PARTITION_NAME_SIZE / 2) {
        throw std::runtime_error("Too long partition name!");
    }
    if (pd.type != PartitionType::LINUX) {
        throw std::runtime_error("Unsupported partition type!");
    }
    if (pd.size_lba == 0) {
        throw std::runtime_error("Partition size cannot be 0!");
    }
    if (pd.first_lba < m_gpt_header.first_usable_lba) {
        throw std::runtime_error("Unable to create partition, it would overwrite GPT table!");
    }
    uint64_t last_lba = pd.first_lba + pd.size_lba - 1;
    if (last_lba > m_gpt_header.last_usable_lba) {
        throw std::runtime_error("Unable to create partition, it would overwrite backup GPT table!");
    }
    // check if new partition would overlap existing one
    for (unsigned i = 0; i < GPT_PARTITION_COUNT; ++i) {
        if (i == id) {
            // skip partition we overwrite
            continue;
        }
        if ((pd.first_lba >= m_gpt_table[i].first_lba && pd.first_lba <= m_gpt_table[i].last_lba)
            || (last_lba >= m_gpt_table[i].first_lba && last_lba <= m_gpt_table[i].last_lba)
            || (pd.first_lba < m_gpt_table[i].first_lba && last_lba > m_gpt_table[i].last_lba)) {

            throw std::runtime_error("Unable to create partition, it would overwrite another partition!");
        }
    }

    m_is_valid = false;
    m_gpt_table[id] = get_linux_partition_entry(pd);
}

void GptPartitionTable::remove_partition(unsigned id) {
    // validate params
    if (id > GPT_PARTITION_COUNT) {
        throw std::runtime_error("Unsupported (too large) partition ID!");
    }
    m_is_valid = false;

    m_gpt_table[id] = GptPartitionData{};
}

uint64_t GptPartitionTable::get_empty_region(uint64_t size_lba) const {

    if (size_lba == 0) {
        throw std::runtime_error("Invalid size (0) of the requested region!");
    }
    uint64_t max_size_lba = m_gpt_header.last_usable_lba - m_gpt_header.first_usable_lba + 1;
    if (size_lba > max_size_lba) {
        throw std::runtime_error("Requested size (" + std::to_string(size_lba)
            + " LBAs) is larger than the size of the drive (" + std::to_string(max_size_lba) + " LBAs)!");
    }

    // section: first and last LBA
    using Section = std::pair<uint64_t, uint64_t>;
    std::vector<Section> sections{};

    // gather starts and ends of partition
    auto partition_ids = get_partition_ids();
    for (const auto partition_id : partition_ids) {
        sections.push_back(std::make_pair(m_gpt_table[partition_id].first_lba, m_gpt_table[partition_id].last_lba));
    }

    // add borders of the empty space
    sections.push_back(std::make_pair(0, m_gpt_header.first_usable_lba - 1));
    sections.push_back(std::make_pair(m_gpt_header.last_usable_lba + 1, m_gpt_header.backup_lba + 1));

    // sort with respect to the first LBA
    std::sort(sections.begin(), sections.end(), [](const Section& lhs, const Section& rhs){return lhs.first < rhs.first;});

    // check if distances between sections are large enough
    for (unsigned i = 1; i < sections.size(); ++i) {
        uint64_t diff = sections[i].first - sections[i - 1].second - 1;
        if (diff >= size_lba) {
            return sections[i - 1].second + 1;
        }
    }
    throw std::runtime_error(std::string{"No free regions of size "} + std::to_string(size_lba) + " LBAs available!");
}


uint64_t GptPartitionTable::get_max_usable_blocks() const {
    return m_gpt_header.last_usable_lba - m_gpt_header.first_usable_lba + 1;
}

void GptPartitionTable::reset(uint64_t drive_size_lba) {

    m_is_valid = false;

    // prepare MBR protective partition
    PartitionData mbr_protective{};
    mbr_protective.type = PartitionType::MBR_PROTECTIVE;
    m_mbr.reset(drive_size_lba);
    m_mbr.set_partition(0, mbr_protective);

    // prepare GPT header - default values
    m_gpt_header.header_size = GPT_HEADER_DATA_SIZE;
    m_gpt_header.current_lba = GPT_HEADER_LBA;
    m_gpt_header.partition_table_lba = GPT_HEADER_LBA + GPT_HEADER_SIZE_LBA;
    m_gpt_header.num_of_partitions = GPT_PARTITION_COUNT;
    m_gpt_header.partition_entry_size = GPT_PARTITION_DATA_SIZE;

    // prepare GPT header - calculated values

    // header copy is always on the last LBA
    m_gpt_header.backup_lba = drive_size_lba - 1;
    uint64_t gpt_table_size_lba = integer_divide_and_ceil(GPT_TABLE_SIZE_BYTES, m_bytes_per_lba);
    // first usable LBA is the first LBA after the GPT table entries
    m_gpt_header.first_usable_lba = GPT_HEADER_LBA + GPT_HEADER_SIZE_LBA + gpt_table_size_lba + 1;
    // last usable LBA is the last LBA just before the backup GPT table entries
    m_gpt_header.last_usable_lba = drive_size_lba - 1 - GPT_HEADER_SIZE_LBA - gpt_table_size_lba;
    m_gpt_header.disk_guid = make_v1_uuid();

    // prepare GPT table entries
    auto empty_partition = GptPartitionData{};
    empty_partition.type_guid = GPT_EMPTY_GUID;
    for (auto& table_entry : m_gpt_table) {
        table_entry = empty_partition;
    }

    // update remaining fields
    update();
}

void GptPartitionTable::update() {

    m_mbr.update();

    // reset some fields with hardcoded/default values
    m_gpt_header.signature = GPT_SIGNATURE;
    m_gpt_header.revision = GPT_REVISION_1_0;

    // recalculate table CRC
    auto table_buffer = gpt_table_to_bb(m_gpt_table);
    m_gpt_header.partition_table_crc = crc::Crc32(table_buffer.data(), GPT_PARTITION_DATA_SIZE * GPT_PARTITION_COUNT);

    // recalculate header CRC, original value has to be zeroed before calculation
    m_gpt_header.header_crc = 0;
    auto header_buffer = gpt_header_to_bb(m_gpt_header);
    m_gpt_header.header_crc = crc::Crc32(header_buffer.data(), GPT_HEADER_DATA_SIZE);

    m_is_valid = true;
}

bool GptPartitionTable::is_valid() const {
    return m_is_valid && m_mbr.is_valid();
}

bool GptPartitionTable::check_crc() const {

    auto expected_header_crc = m_gpt_header.header_crc;
    auto expected_table_crc = m_gpt_header.partition_table_crc;

    auto gpt_header_copy = m_gpt_header;
    gpt_header_copy.header_crc = 0; // this has to be set to 0 for header CRC calculation

    auto header_buffer = gpt_header_to_bb(gpt_header_copy);
    auto table_buffer = gpt_table_to_bb(m_gpt_table);

    uint32_t calculated_header_crc = crc::Crc32(header_buffer.data(), GPT_HEADER_DATA_SIZE);
    uint32_t calculated_table_crc = crc::Crc32(table_buffer.data(), GPT_PARTITION_DATA_SIZE * GPT_PARTITION_COUNT);

    return (expected_header_crc == calculated_header_crc && expected_table_crc == calculated_table_crc);
}
