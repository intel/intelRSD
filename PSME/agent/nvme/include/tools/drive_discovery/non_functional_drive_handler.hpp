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
 * Drive handler implementation for non functional drives (i.e. actions will throw, drives will have critical health)
 */
class NonFunctionalDriveHandler : public BaseDriveHandler {
public:

    /*!
     * @brief Constructs a valid handler
     * @param name Name of the drive device
     * @param nvme_interface NVMe interface for querying NVMe commands
     */
    NonFunctionalDriveHandler(const std::string& name, std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface)
        : BaseDriveHandler(name), m_nvme_interface(nvme_interface)  {
    }

    /*!
     * @brief virtual Destructor
     */
    virtual ~NonFunctionalDriveHandler();

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

    /*! NVMe interface used to query NVMe commands */
    std::shared_ptr<::nvme::AbstractNvmeInterface> m_nvme_interface;

    /*! Primary drive controller data */
    ::nvme::ControllerData m_controller_data{};

};

}
}
}
