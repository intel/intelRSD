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
 * @file partition/gpt_partition_table.hpp
 */

#pragma once

#include "partition/abstract_partition_table.hpp"
#include "partition/mbr_partition_table.hpp"
#include "utils/byte_buffer.hpp"

#include <array>

namespace partition {

/*!
 * @brief AbstractPartitionTable implementation for the GPT (Guid Partition Table) labels
 */
class GptPartitionTable : public AbstractPartitionTable {
public:

    /*!
     * @brief Constructs a GptPartitionTable
     * @param lba_size Number of bytes per single LBA of the drive
     */
    GptPartitionTable(uint32_t lba_size): m_bytes_per_lba(lba_size) { }

    virtual ~GptPartitionTable();

    /*! Pure virtual methods implemented from the parent */
    virtual void load(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) override;
    virtual void save(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) const override;
    virtual std::vector<unsigned> get_partition_ids() const override;
    virtual unsigned get_available_partition_id() const override;
    virtual PartitionData get_partition(unsigned id) const override;
    virtual void set_partition(unsigned id, const PartitionData& pd) override;
    virtual void remove_partition(unsigned id) override;
    virtual uint64_t get_empty_region(uint64_t size_lba) const override;
    virtual uint64_t get_max_usable_blocks() const override;
    virtual void reset(uint64_t drive_size_lba) override;
    virtual void update() override;
    virtual bool is_valid() const override;
protected:

    /*! GPT header location (LBAs) */
    static constexpr uint64_t GPT_HEADER_LOCATION = 1;
    /*! Size of the GPT header (bytes) */
    static constexpr unsigned GPT_HEADER_DATA_SIZE = 92;
    /*! Size of the GPT partition entry (bytes) */
    static constexpr unsigned GPT_PARTITION_DATA_SIZE = 128;
    /*! Minimum number of partitions that have to be supported */
    static constexpr unsigned GPT_PARTITION_COUNT = 128;
    /*! Size of the GUID field (bytes) */
    static constexpr unsigned GPT_GUID_SIZE = 16;
    /*! Number of bytes in the partition name field */
    static constexpr unsigned GPT_PARTITION_NAME_SIZE = 72;
    /*! GPT header signature = "EFI PART" */
    static constexpr uint64_t GPT_SIGNATURE = 0x5452415020494645;
    /*! GPT header revision */
    static constexpr uint32_t GPT_REVISION_1_0 = 0x00010000;
    /*! UUID representing an empty partition empty */
    static constexpr char GPT_EMPTY_GUID[] = "00000000-0000-0000-0000-000000000000";
    /*! UUID representing a generic Linux partition */
    static constexpr char GPT_PARTITION_TYPE_UUID_LINUX[] = "0FC63DAF-8483-4772-8E79-3D69D8477DE4";
    /*! Default LBA location of the GPT header */
    static constexpr uint64_t GPT_HEADER_LBA = 1;
    /*! Size of the GPT header in LBAs */
    static constexpr uint64_t GPT_HEADER_SIZE_LBA = 1;
    /*! Size of the GPT table in bytes */
    static constexpr uint64_t GPT_TABLE_SIZE_BYTES = 16384;


    /*! Header of the GPT table */
    struct GptHeaderData {
        /*! GPT signature, should have "EFI PART" value*/
        uint64_t signature{};
        /*! GPT revision */
        uint32_t revision{};
        /*! Total size of the header (bytes), usually 92 bytes */
        uint32_t header_size{};
        /*! CRC32 checksum of the GPT header */
        uint32_t header_crc{};
        /*! Reserved fields, must be 0 */
        uint8_t reserved_20[4]{};
        /*! Location of the header (LBA) */
        uint64_t current_lba{};
        /*! Location of the backup header (LBA), usually the last LBA of the drive*/
        uint64_t backup_lba{};
        /*! First usable LBA, usually the first LBA after the partition table entries */
        uint64_t first_usable_lba{};
        /*! Last usable LBA, last LBA before the backup partition table */
        uint64_t last_usable_lba{};
        /*! GUID of the drive */
        std::string disk_guid{GPT_EMPTY_GUID};
        /*! Location of the first entry of the partition table (LBA) */
        uint64_t partition_table_lba{};
        /*! Total number of partitions */
        uint32_t num_of_partitions{};
        /*! Size of a single partition entry (bytes), usually 128 bytes */
        uint32_t partition_entry_size{};
        /*! CRC32 checksum of the partition table */
        uint32_t partition_table_crc{};
    };

    /*! Single GPT partition entry */
    struct GptPartitionData {
        /*! GUID of the partition type */
        std::string type_guid{GPT_EMPTY_GUID};
        /*! GUID of the partition */
        std::string guid{GPT_EMPTY_GUID};
        /*! First LBA of the partition */
        uint64_t first_lba{};
        /*! Last LBA (inclusive) of the partition */
        uint64_t last_lba{};
        /*! Partition flags */
        uint64_t flags{};
        /*! Name of the partition, UTF-16LE code */
        std::string name{};
    };

    /*! Gpt partition table definition */
    using GptTableData = std::array<GptPartitionData, GPT_PARTITION_COUNT>;


    /*! Converts ByteBuffer to the GPT header */
    static GptHeaderData bb_to_gpt_header(const utils::ByteBuffer&);

    /*! Converts ByteBuffer to the GPT partition table */
    static GptTableData bb_to_gpt_table(const utils::ByteBuffer&);

    /*! Converts GptHeaderData to the ByteBuffer */
    static utils::ByteBuffer gpt_header_to_bb(const GptHeaderData&);

    /*! Converts GptTableData to the ByteBuffer */
    static utils::ByteBuffer gpt_table_to_bb(const GptTableData&);


    /*! Extracts GUID from ByteBuffer's specific location */
    static std::string get_guid(const utils::ByteBuffer&, uint64_t);

    /*! Extracts name from ByteBuffer's specific location */
    static std::string get_name(const utils::ByteBuffer&, uint64_t);

    /*! Writes GUID to a specific location of the ByteBuffer */
    static void set_guid(utils::ByteBuffer&, uint64_t, const std::string&);

    /*! Writes name to a specific location of the ByteBuffer */
    static void set_name(utils::ByteBuffer&, uint64_t, const std::string&);


    /*!
     * @brief Checks if CRCs in the GPT header are correct
     * @return True if CRCs are correct
     */
    bool check_crc() const;

    /*!
     * @brief Loads GPT header and GPT table
     * @param target Name of the target device
     * @param interface Interface used for reading
     * @param header_lba Location offset of the GPT header (in LBA)
     * @param is_primary If true, primary GPT table is read, otherwise the backup copy instead
     */
    void load_gpt(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface, uint64_t header_lba,
        bool is_primary);

    /*!
     * @brief Loads MBR table and checks it for the MBR protective partition
     * @param target Name of the target device
     * @param interface Interface used for reading
     */
    void load_mbr(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface);

    /*!
     * @brief Builds a valid Linux GPT partition table entry based on the PartitionData struct
     * @param pd PartitionData struct describing a partition
     * @return Linux GPT partition table entry
     */
    static GptPartitionData get_linux_partition_entry(const PartitionData& pd);


    /*! Header of the GPT */
    GptHeaderData m_gpt_header{};

    /*! GPT partition table */
    GptTableData m_gpt_table{{}};

    /*! MBR partition table required for MBR Protective partitions*/
    MbrPartitionTable m_mbr{};

    /*! Number of bytes per single LBA */
    std::uint32_t m_bytes_per_lba{};

};

}
