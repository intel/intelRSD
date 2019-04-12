/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file bdev.hpp
 */

#pragma once



#include <string>
#include "supported_io_types.hpp"
#include "assigned_rate_limits.hpp"

#include "json-wrapper/json-wrapper.hpp"
#include "spdk/json_optional.hpp"



namespace spdk {

namespace model {

class BdevProductName {
public:
    BdevProductName() = delete;


    static constexpr const char MALLOC_DISK[] = "Malloc Disk";
    static constexpr const char LOGICAL_VOLUME[] = "Logical Volume";
    static constexpr const char NVMe_DISK[] = "NVMe disk";

};

class RaidBdevCategory {
public:
    RaidBdevCategory() = delete;


    static constexpr const char ALL[] = "all";
    static constexpr const char ONLINE[] = "online";
    static constexpr const char CONFIGURING[] = "configuring";
    static constexpr const char OFFLINE[] = "offline";
};

class ClearMethod {
public:
    ClearMethod() = delete;

    static constexpr const char WRITE_ZEROES[] = "write_zeroes";
    static constexpr const char NONE[] = "none";
    static constexpr const char UNMAP[] = "unmap";
};

/*!
 * @brief Implementation of Bdev's model based on SPDK
 */
class Bdev {
public:
    static constexpr const char NAME[] = "name";
    static constexpr const char UUID[] = "uuid";
    static constexpr const char PRODUCT_NAME[] = "product_name";
    static constexpr const char ALIASES[] = "aliases";
    static constexpr const char BDEV_BLOCK_SIZE[] = "block_size";
    static constexpr const char NUM_BLOCKS[] = "num_blocks";
    static constexpr const char CLAIMED[] = "claimed";
    static constexpr const char DRIVER_SPECIFIC[] = "driver_specific";
    static constexpr const char SUPPORTED_IO_TYPES[] = "supported_io_types";
    static constexpr const char QOS_IOS_PER_SEC[] = "qos_ios_per_sec";
    static constexpr const char ASSIGNED_RATE_LIMITS[] = "assigned_rate_limits";


    /*!
     * @brief Gets the Bdev's name
     * @return Bdev's name
     */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Sets the Bdev's name
     * @param Unique name for this block device.
     */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
    * @brief Gets Bdev's UUID
    * @return Bdev's UUID
    */
    const JsonOptional<std::string>& get_uuid() const {
        return m_uuid;
    }


    /*!
     * @brief Sets the Bdev's UUID
     * @param uuid value of Bdev's UUID
     */
    void set_uuid(const JsonOptional<std::string>& uuid) {
        m_uuid = uuid;
    }


    /*!
     * @brief Gets unique product name for this kind of block device
     * @return Bdev's product name
     */
    const std::string& get_product_name() const {
        return m_product_name;
    }


    /*!
     * @brief Sets the Bdev's product name
     * @param product_name unique product name
     */
    void set_product_name(const std::string& product_name) {
        m_product_name = product_name;
    }


    /*!
    * @brief Gets Bdev's aliases
    * @return Bdev's aliases
    */
    const std::vector<std::string>& get_aliases() const {
        return m_aliases;
    }


    /*!
     * @brief Sets the Bdev's aliases
     * @param aliases value of Bdev's aliases
     */
    void set_aliases(const std::vector<std::string>& aliases) {
        m_aliases = aliases;
    }


    /*!
     * @brief Gets size of a block
     * @return Bdev's block size
     */
    std::uint32_t get_block_size() const {
        return m_block_size;
    }


    /*!
     * @brief Sets the Bdev's block size
     * @param block_size size of a block
     */
    void set_block_size(uint32_t block_size) {
        m_block_size = block_size;
    }


    /*!
     * @brief Gets quantity of the blocks in the Bdev
     * @return Bdev's number of blocks
     */
    std::uint64_t get_num_blocks() const {
        return m_num_blocks;
    }


    /*!
     * @brief Sets the Bdev's number of blocks
     * @param num_blocks number of blocks
     */
    void set_num_blocks(std::uint64_t num_blocks) {
        m_num_blocks = num_blocks;
    }


    /*!
     * @brief Gets info if Bdev is claimed
     * @return Returns true if Bdev is claimed
     */
    bool is_claimed() const {
        return m_claimed;
    }


    /*!
     * @brief Sets if Bdev is claimed
     * @param claimed true if claimed, otherwise false
     */
    void set_claimed(bool claimed) {
        m_claimed = claimed;
    }


    /*!
     * @brief Gets Bdev's driver specific object
     * @return Returns Bdev's driver specific
     */
    const json::Json& get_driver_specific() const {
        return m_driver_specific;
    }


    /*!
     * @brief Sets Bdev's driver specific object
     * @param driver Bdev's driver specific object
     */
    void set_driver_specific(const json::Json& driver) {
        m_driver_specific = driver;
    }


    /*!
     * @brief Gets Bdev's driver specific object by driver name
     * @return Returns Bdev's driver specific object
     */
    template<typename T>
    JsonOptional<T> get_driver_specific() const {
        json::Json json = m_driver_specific.value(T::DRIVER_NAME, json::Json{});
        return json.get<JsonOptional<T>>();
    }


    /*!
     * @brief Sets Bdev's driver specific object by driver name
     * @param driver_specific Bdev's driver specific object
     */
    template<typename T>
    void set_driver_specific(const JsonOptional<T>& driver_specific) {
        m_driver_specific[T::DRIVER_NAME] = driver_specific;
    }


    /*!
     * @brief Check whether the block device supports the I/O type like read, write, flush, unmap.
     * @return SupportedIoTypes object
     */
    const SupportedIoTypes& get_supported_io_types() const {
        return m_supported_io_types;
    }


    /*!
     * @brief Sets supported I/O types
     * @param supported_io_types info about supported IO included in SupportedIoTypes object
     */
    void set_supported_io_types(const SupportedIoTypes& supported_io_types) {
        m_supported_io_types = supported_io_types;
    }


    /*!
     * @brief Get IOs per second of block device for the QoS rate limiting.
     * @return IOs per second
     */
    JsonOptional<std::uint64_t> get_qos_ios_per_sec() const {
        return m_qos_ios_per_sec;
    }


    /*!
     * @brief Sets IOs per second
     * @param qos_ios_per_sec
     */
    void set_qos_ios_per_sec(JsonOptional<std::uint64_t> qos_ios_per_sec) {
        m_qos_ios_per_sec = qos_ios_per_sec;
    }

    /*!
     * @brief Get AssignedRateLimit
     * @return AssignedRateLimit
     */
    JsonOptional<AssignedRateLimits> get_assigned_rate_limits() const {
        return m_assigned_rate_limits;
    }


    /*!
     * @brief Sets AssignedRateLimit
     * @param AssignedRateLimit
     */
    void set_assigned_rate_limits(const JsonOptional<AssignedRateLimits>& assigned_rate_limits) {
        m_assigned_rate_limits = assigned_rate_limits;
    }


    /*!
     * @brief Lists current info about Bdev
     * @return info about Bdev as a string
     */
    std::string to_string() const;


private:
    std::string m_name{};
    JsonOptional<std::string> m_uuid{};
    std::string m_product_name{};
    std::vector<std::string> m_aliases{};
    std::uint32_t m_block_size{};
    std::uint64_t m_num_blocks{};
    bool m_claimed{};
    json::Json m_driver_specific{};
    SupportedIoTypes m_supported_io_types{};
    JsonOptional<AssignedRateLimits> m_assigned_rate_limits{};
    /*Not supported in SPDK v18.10.1*/
    JsonOptional<std::uint64_t> m_qos_ios_per_sec{};
};


/*!
 * @brief converts from JSON to Bdev
 * @param json json object
 * @param bdev Bdev object
 */
void from_json(const json::Json& json, Bdev& bdev);


/*!
 * @brief converts from Bdev to Json
 * @param json Json object
 * @param bdev Bdev object
 */
void to_json(json::Json& json, const Bdev& bdev);

}
}


