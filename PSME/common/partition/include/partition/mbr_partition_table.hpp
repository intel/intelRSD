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
 * @file partition/mbr_partition_table.hpp
 */

#pragma once

#include "partition/abstract_partition_table.hpp"
#include "utils/byte_buffer.hpp"

namespace partition {

/*!
 * @brief Simple MBR partition table with implemented support for 'MBR Protective' partitions only
 */
class MbrPartitionTable : public AbstractPartitionTable {
public:

    MbrPartitionTable() {}

    virtual ~MbrPartitionTable();

    /*! Pure virtual methods implemented from the parent */
    virtual void load(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) override;
    virtual void save(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) const override;
    virtual std::vector<unsigned> get_partition_ids() const override;
    virtual unsigned get_available_partition_id() const override;
    virtual PartitionData get_partition(unsigned id) const override;
    virtual void set_partition(unsigned id, const PartitionData& pd) override;
    virtual void remove_partition(unsigned id) override;
    [[noreturn]] virtual uint64_t get_empty_region(uint64_t size_lba) const override;
    [[noreturn]] virtual uint64_t get_max_usable_blocks() const override;
    virtual void reset(uint64_t drive_size_lba) override;
    virtual void update() override;

protected:

    /*! Location of the MBR on the drive */
    static constexpr unsigned MBR_OFFSET = 0;
    /*! Total size of MBR (bytes) */
    static constexpr unsigned MBR_DATA_SIZE = 512;
    /*! Total size of the bootstrap region of the MBR (bytes) */
    static constexpr unsigned MBR_BOOTSTRAP_SIZE = 446;
    /*! Number of partition entries present in the MBR */
    static constexpr unsigned MBR_PARTITION_ENTRY_COUNT = 4;
    /*! Location offset of partition entries within the MBR (bytes) */
    static constexpr unsigned MBR_PARTITION_ENTRY_OFFSET[MBR_PARTITION_ENTRY_COUNT] = {446, 462, 478, 494};
    /*! Location offset of the boot signature field */
    static constexpr unsigned MBR_BOOT_SIGNATURE_OFFSET = 510;
    /*! Expected value of the boot signature field */
    static constexpr uint16_t MBR_BOOT_SIGNATURE = 0xAA55;
    /*! Partition type of an empty partition entry */
    static constexpr uint8_t MBR_PARTITION_TYPE_NONE = 0x00;

    /*!
     * Partition type of 'MBR Protective' partitions. These partitions are recognized as 'unknown type'
     * and partitioning programs will ignore it. This is to protect GPT (GUID Partition Table) data from
     * programs not supporting it.
     */
    static constexpr uint8_t MBR_PARTITION_TYPE_PROTECTIVE = 0xEE;

    /*!
     * The following three consts are used to describe MBR Protective (0xEE) partitions: their first/last
     * sectors (using cyllinder-head-sector (CHS) addressing) and first available LBA. It is used to create
     * one dummy partition that takes as much space on the drive as possible.
     */
    static constexpr uint8_t MBR_PROTECTIVE_FIRST_SECTOR_CHS[] = {0x00, 0x01, 0x00};
    static constexpr uint8_t MBR_PROTECTIVE_LAST_SECTOR_CHS[] = {0xFE, 0xFF, 0xFF};
    static constexpr uint32_t MBR_PROTECTIVE_FIRST_AVAILABLE_LBA = 0x00000001;

    /*! Struct representing single MBR partition data */
    struct MbrPartitionData final {
        /*! Partition status, not used in MBR Protective partitions*/
        uint8_t status{};
        /*! First sector of the partition (cyllinder-head-sector addressing) */
        uint8_t first_sector_chs[3]{};
        /*! Type of the partition*/
        uint8_t partition_type{};
        /*! Last sector of the partition (cyllinder-head-sector addressing) */
        uint8_t last_sector_chs[3]{};
        /*! Location of the first usable partition sector (in LBA addressing) */
        uint32_t first_sector_lba{};
        /*! Number of sectors within the partition */
        uint32_t num_of_sectors{};
    };

    /*! Struct representing data stored in MBR */
    struct MbrData final {
        /*! Bootstrap code */
        uint8_t bootstrap[MBR_BOOTSTRAP_SIZE]{};
        /*! Primary MBR partition entries */
        MbrPartitionData partition_data[MBR_PARTITION_ENTRY_COUNT]{};
        /*! Boot signature */
        uint16_t boot_signature{};
    };

    /*! Converts ByteBuffer to the MbrData struct */
    static MbrData bb_to_mbr(const utils::ByteBuffer&);

    /*! Converts MbrData struct to the ByteBuffer */
    static utils::ByteBuffer mbr_to_bb(const MbrData&);

    /*! MBR data representing the current partition table */
    MbrData m_mbr{};

};

}
