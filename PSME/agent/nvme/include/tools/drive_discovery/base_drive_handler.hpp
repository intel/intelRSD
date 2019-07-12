/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
namespace nvme {
namespace tools {

/*!
 * Interface for drive handlers. Their responsibility is to provide common actions
 * performed on drives (read properties, manage volumes etc).
 */
class BaseDriveHandler {
public:

    struct NvmeFeatures {
        OptionalField<bool> latency_tracking_enabled{};
    };

    /*! Struct representing drive data */
    struct DriveData final {
        OptionalField<std::string> serial_number{};
        OptionalField<std::string> model_number{};
        OptionalField<std::string> firmware_revision{};
        OptionalField<std::string> manufacturer_number{};
        OptionalField<uint64_t> size_lba{};
        OptionalField<uint64_t> available_size_lba{};
        OptionalField<uint32_t> block_size_bytes{};
        NvmeFeatures features{};
        std::vector<uint32_t> namespaces{};
    };

    /*! Struct representing volume data */
    struct VolumeData final {
        OptionalField<uint64_t> size_lba{};
        OptionalField<uint32_t> block_size_bytes{};
        OptionalField<std::string> uuid{};
    };

    /*! Struct representing drive SMART data */
    struct SmartData final {
        OptionalField<uint8_t> critical_warnings{};
        OptionalField<uint16_t> composite_temperature{};
        OptionalField<uint8_t> available_spare{};
        OptionalField<uint8_t> available_spare_threshold{};
        OptionalField<uint8_t> percentage_used{};
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

    /*! Struct representing Intel drive's latency histogram */
    struct LatencyData final {
        struct HistogramGroup {
            OptionalField<std::uint32_t> min_range_us{};
            OptionalField<std::uint32_t> max_range_us{};
            OptionalField<std::uint32_t> step_us{};
            std::vector<std::uint32_t> buckets{};
        };

        struct HistogramData {
            HistogramGroup group_1st{};
            HistogramGroup group_2nd{};
            HistogramGroup group_3rd{};
            HistogramGroup group_4th{};
            HistogramGroup group_5th{};
            HistogramGroup group_6th{};
        };

        HistogramData write_histogram{};
        HistogramData read_histogram{};
    };


    /*!
     * @brief Constructs a valid BaseDriveHandler
     * @param name Name of the drive device
     */
    BaseDriveHandler(const std::string& name) : m_name(name) {}


    /*!
     * @brief virtual Destructor
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
     * @brief Returns list of ids of all volumes on the drive
     * @return Ids of all volumes on the drives
     */
    virtual std::vector<std::string> get_volumes() const = 0;


    /*!
     * @brief Creates a volume
     * @param size_bytes Size of the volume to be created
     * @return Id of the created volume
     */
    virtual std::string create_volume(const uint64_t size_bytes) const = 0;


    /*!
     * @brief Removes a volume
     * @param id Id of the volume to be removed
     */
    virtual void remove_volume(const std::string& id) const = 0;


    /*!
     * @brief Gets data of the volume
     * @param id Id of the queried volume
     * @return Data describing the volume
     */
    virtual VolumeData get_volume_data(const std::string& id) const = 0;


    /*!
     * @brief Get drive SMART data.
     * @return Drive SMART data.
     */
    virtual SmartData load_smart_info() const = 0;


    /*!
     * @brief Get drive latency histogram data.
     * @return Drive latency histogram data.
     */
    virtual LatencyData load_latency_histogram() const = 0;


    /*!
     * @brief Initializes drive - prepares it for volume creation/removal
     */
    virtual void initialize_drive() = 0;


    /*!
     * @brief Clears volume data
     * @param id Id of the volume to be cleared
     * @param fast_flag If true, fast erase will be performed, slow otherwise
     */
    virtual void erase_volume(const std::string& id, bool fast_mode) = 0;


protected:

    /*! Drive name */
    std::string m_name{};

    /*! Global log page id */
    static constexpr uint32_t global_log_page_id = 0xFFFFFFFF;

};

}
}
}
