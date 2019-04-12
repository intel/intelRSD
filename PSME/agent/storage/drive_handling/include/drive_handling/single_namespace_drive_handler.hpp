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



#include "drive_handling/base_drive_handler.hpp"
#include "nvme/abstract_nvme_interface.hpp"

#include <mutex>

namespace agent {
namespace storage {

/*!
 * Drive handler implementation for single namespace drives
 */
class SingleNamespaceDriveHandler : public BaseDriveHandler {
public:

    /*!
     * @brief Constructs a valid handler
     * @param name Name of the drive device
     * @param nvme_interface NVMe interface for querying NVMe commands
     */
    SingleNamespaceDriveHandler(const std::string& name,
                                std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface)
        : BaseDriveHandler(name), m_nvme_interface(nvme_interface) {
    }

    /*!
     * @brief Virtual destructor.
     */
    virtual ~SingleNamespaceDriveHandler();

    /*! Implemented pure virtual methods from the base class */
    virtual void load() override;
    virtual DriveData get_drive_data() const override;
    virtual SmartData get_smart_info() const override;

protected:

    /*!
     * @brief Template method for obtaining ID of the primary namespace
     * @return Namespace ID
     */
    virtual std::uint32_t get_primary_namespace_id() const;

    /*! NVMe interface used to query NVMe commands */
    std::shared_ptr<::nvme::AbstractNvmeInterface> m_nvme_interface;

    /*! LBA size in bytes */
    std::uint32_t m_lba_size{};

    /*! Primary drive controller data */
    ::nvme::ControllerData m_controller_data{};

    /*! Primary namespace Id */
    std::uint32_t m_namespace_id{};

    /*! Primary drive namespace data */
    ::nvme::NamespaceData m_namespace_data{};

    mutable std::mutex m_mutex{};
};

}
}
