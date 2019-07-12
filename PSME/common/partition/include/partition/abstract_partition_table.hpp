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
 * @file partition/abstract_partition_table.hpp
 */

#pragma once

#include "partition/abstract_drive_interface.hpp"

#include <memory>

namespace partition {

/*! Represents type of the partition */
enum class PartitionType : uint8_t {
    UNKNOWN,
    MBR_PROTECTIVE,
    LINUX
};

/*! Describes a single partition */
struct PartitionData final {
    /*! Location of the first LBA of the partition */
    uint64_t first_lba{};
    /*! Total size of the partition (in LBAs) */
    uint64_t size_lba{};
    /*! Type of the partition */
    PartitionType type{};
    /*! Name of the partition */
    std::string name{};
    /*! Unique identifier of this partition */
    std::string uuid{};
};

/*!
 * @brief Base class for all partition tables
 */
class AbstractPartitionTable {
public:

    virtual ~AbstractPartitionTable();

    /*! Exception thrown when partition table is full and no free Id's are available */
    class IdUnavailable final : public std::runtime_error {
    public:
        IdUnavailable():std::runtime_error{"There are no empty partition entries in the table."} {}
        IdUnavailable(const IdUnavailable&) = default;
        IdUnavailable(IdUnavailable&&) = default;
        IdUnavailable& operator=(const IdUnavailable&) = default;
        IdUnavailable& operator=(IdUnavailable&&) = default;

        virtual ~IdUnavailable();
    };

    /*!
     * @brief Loads partition structure from the provided interface
     * @param target Target device
     * @param interface Interface used for reading
     */
    virtual void load(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) = 0;

    /*!
     * @brief Saves partition structure using the provided interface
     * @param target Target device
     * @param interface Interface used for writing
     */
    virtual void save(const std::string& target, std::shared_ptr<AbstractDriveInterface> interface) const = 0;

    /*!
     * @brief Returns list of all partitions found
     * @return List of all partitions
     */
    virtual std::vector<unsigned> get_partition_ids() const = 0;

    /*!
     * @brief Returns a first empty partition slot that can be written
     * @return Empty partition slot
     * @throws IdUnavailable if no free partitions slots are availabe
     */
    virtual unsigned get_available_partition_id() const = 0;

    /*!
     * @brief Returns partition data
     * @param id Id of the queried partition
     * @return Struct describing the partition properties
     */
    virtual PartitionData get_partition(unsigned id) const = 0;

    /*!
     * @brief Modifies partition according to the provided parameters
     * @param id Id of the modified partition
     * @param pd Descriptor containing new values for the partition
     */
    virtual void set_partition(unsigned id, const PartitionData& pd) = 0;

    /*!
     * @brief Removes specific partition
     * @param id Id of the removed partition
     */
    virtual void remove_partition(unsigned id) = 0;

    /*!
     * @brief Returns position of a free region of a specific length
     * @param size_lba Size of the requested free region
     * @return LBA of the first empty region
     */
    virtual uint64_t get_empty_region(uint64_t size_lba) const = 0;

    /*!
     * @brief Returns total number of usable blocks
     * @return Size in LBAs
     */
    virtual uint64_t get_max_usable_blocks() const = 0;

    /*!
     * @brief Creates an empty, but valid partition table for a drive using provided parameters
     * @param drive_size_lba Size of the drive in LBAs
     */
    virtual void reset(uint64_t drive_size_lba) = 0;

    /*!
     * @brief Performs any additional operations that are needed after changing the partition table e.g. CRC calculation
     */
    virtual void update() = 0;

    /*!
     * @brief Returns true if data stored by the class is valid and can be written to the drive
     * @return True if valid data is stored in the class
     */
    virtual bool is_valid() const {
        return m_is_valid;
    }

protected:
    bool m_is_valid{false};

};

}
