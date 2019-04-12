/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file base_drive_handler.hpp
 */

#pragma once



#include "agent-framework/module/utils/optional_field.hpp"

#include <string>
#include <cstdint>



namespace agent {
namespace storage {

/*!
 * Interface for drive handlers. Their responsibility is to provide common actions
 * performed on drives (read properties, etc).
 */
class BaseDriveHandler {
public:
    using SPtr = std::shared_ptr<BaseDriveHandler>;

    /*! Struct representing drive data */
    struct DriveData final {
        OptionalField<std::string> serial_number{};
        OptionalField<std::string> model_number{};
        OptionalField<std::string> firmware_revision{};
        OptionalField<std::string> manufacturer_number{};
        OptionalField<std::string> storage_protocol{};
        OptionalField<std::string> drive_type{};
        OptionalField<std::uint32_t> rpm{};
        OptionalField<std::uint64_t> size_lba{};
        OptionalField<std::uint32_t> block_size_bytes{};
        std::vector<std::string> namespaces{}; /* List of device names with namespaces */
    };

    /*! Struct representing drive SMART data */
    struct SmartData final {
        OptionalField<std::uint8_t> critical_warnings{};
        OptionalField<std::uint16_t> composite_temperature{};
        OptionalField<std::uint8_t> available_spare{};
        OptionalField<std::uint8_t> available_spare_threshold{};
        OptionalField<std::uint8_t> percentage_used{};
        OptionalField<long double> data_unit_read{};
        OptionalField<long double> data_unit_written{};
        OptionalField<long double> host_read_commands{};
        OptionalField<long double> host_write_commands{};
        OptionalField<long double> controller_busy_time{};
        OptionalField<long double> power_cycles{};
        OptionalField<long double> power_on_hours{};
        OptionalField<long double> unsafe_shutdowns{};
        OptionalField<long double> media_data_integrity_errors{};
        OptionalField<long double> number_of_error_log_entries{};
    };


    /*!
     * @brief Constructs a valid BaseDriveHandler
     * @param name Name of the drive device
     */
    BaseDriveHandler(const std::string& name) : m_name(name) {}


    /*!
     * @brief Virtual destructor
     */
    virtual ~BaseDriveHandler();


    /*!
     * @brief Loads drive information
     */
    virtual void load() = 0;


    /*!
     * @brief Gets data of the handled drive
     * @return Data describing the handled drive
     */
    virtual DriveData get_drive_data() const = 0;


    /*!
     * @brief Get drive SMART data.
     * @return Drive SMART data.
     */
    virtual SmartData get_smart_info() const = 0;


protected:

    /*! Drive name */
    std::string m_name{};

    /*! Global log page id */
    static const constexpr std::uint32_t GLOBAL_LOG_PAGE_ID = 0xFFFFFFFF;

    static const constexpr char SSD_TYPE[] = "SSD";
    static const constexpr char HDD_TYPE[] = "HDD";
    static const constexpr char NVME_PROTOCOL[] = "NVMe";
};

}
}
