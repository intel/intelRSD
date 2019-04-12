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
 * @file single_namespace_drive_handler.hpp
 */

#pragma once

#include "tools/drive_discovery/base_drive_handler.hpp"
#include "nvme/abstract_nvme_interface.hpp"
#include "partition/abstract_partition_table.hpp"
#include "partition/abstract_drive_interface.hpp"

#include <mutex>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * Drive handler implementation for single namespace drives
 */
class SingleNamespaceDriveHandler : public BaseDriveHandler {
public:

    /*!
     * @brief Constructs a valid handler
     * @param name Name of the drive device
     * @param nvme_interface NVMe interface for querying NVMe commands
     * @param drive_interface Drive interface for reading drives
     */
    SingleNamespaceDriveHandler(const std::string& name, std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface,
            std::shared_ptr<::partition::AbstractDriveInterface> drive_interface)
        : BaseDriveHandler(name), m_nvme_interface(nvme_interface), m_drive_interface(drive_interface) {
    }

    /*!
     * @brief virtual Destructor
     */
    virtual ~SingleNamespaceDriveHandler();

    /*! Implemented pure virtual methods from the base class */
    virtual void load() override;
    virtual DriveData get_drive_data() const override;
    virtual std::vector<std::string> get_volumes() const override;
    virtual std::string create_volume(const uint64_t size_bytes) const override;
    virtual void remove_volume(const std::string& id) const override;
    virtual VolumeData get_volume_data(const std::string& id) const override;
    virtual SmartData load_smart_info() const override;
    virtual LatencyData load_latency_histogram() const override;
    virtual void initialize_drive() override;
    virtual void erase_volume(const std::string& id, bool fast_mode) override;

protected:

    /*!
     * @brief Template method for obtaining ID of the primary namespace
     * @return Namespace ID
     */
    virtual uint32_t get_primary_namespace_id() const;

    /*! NVMe interface used to query NVMe commands */
    std::shared_ptr<::nvme::AbstractNvmeInterface> m_nvme_interface;

    /*! Drive interface used to access raw drive data */
    std::shared_ptr<::partition::AbstractDriveInterface> m_drive_interface;

    /*! LBA size in bytes */
    uint32_t m_lba_size{};

    /*! Primary drive controller data */
    ::nvme::ControllerData m_controller_data{};

    /*! Primary namespace Id */
    uint32_t m_namespace_id{};

    /*! Primary drive namespace data */
    ::nvme::NamespaceData m_namespace_data{};

    /*! Flag for Intel-OEM telemetry feature of Intel SSD */
    OptionalField<bool> m_latency_tracking_feature_enabled{};

    /*! Loaded partition table of the drive */
    std::shared_ptr<::partition::AbstractPartitionTable> m_partition_table{};

    mutable std::mutex m_mutex{};

};

}
}
}
