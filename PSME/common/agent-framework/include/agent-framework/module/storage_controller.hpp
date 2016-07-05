/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file storage_controller.hpp
 * @brief Mass storage controller
 * */

#ifndef AGENT_FRAMEWORK_MODULE_STORAGE_CONTROLLER_HPP
#define AGENT_FRAMEWORK_MODULE_STORAGE_CONTROLLER_HPP

#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module/fru_info.hpp"
#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/module/resources.hpp"
#include "agent-framework/logger_ext.hpp"
#include "json/json.hpp"

#include <string>
#include <vector>
#include <algorithm>

namespace agent_framework {
namespace generic {

using HardDriveSharedPtr = HardDrive::HardDriveSharedPtr;
using HardDriveWeakPtr = HardDrive::HardDriveWeakPtr;

/*! Storage Controller */
class StorageController : public Resources {
    std::string m_interface{};
    uint32_t m_drive_count = 0;
    FruInfo m_fru_info{};

    std::vector<HardDriveSharedPtr> m_hard_drives{};
public:
    /*! StorageController unique pointer */
    using StorageControllerUniquePtr = std::unique_ptr<StorageController>;

    /*! Default constructor */
    StorageController(){}

    /*! Creates Storage Controller unique object */
    static StorageControllerUniquePtr make_storage_controller() {
        return StorageControllerUniquePtr{ new StorageController() };
    }

    /*!
     * Read Storage Controller configuration
     *
     * @param[in] controller_configuration Storage Controller configuration JSON
     * */
    void read_configuration(const json::Value& controller_configuration);

    /*!
     * Returns Storage Controller interface
     *
     * @return Storage Controller interface
     * */
    const std::string& get_interface() const { return m_interface; }

    /*!
     * Returns Storage Controller hard drive count
     *
     * @return Storage COntroller hard drive count
     * */
    const uint32_t& get_drive_count() const { return m_drive_count; }

    /*!
     * @brief Adds new hard drive to storage controller.
     *
     * @param[in] hard_drive  Pointer to new hard drive.
     * */
    void add_hard_drive(const HardDriveSharedPtr& hard_drive) {
        m_hard_drives.push_back(hard_drive);
    }

    /*!
     * @brief Removes hard drive from storage controller.
     *
     * @param[in] hard_drive  Pointer to hard drive to be removed.
     * */
    void remove_hard_drive(const HardDriveSharedPtr& hard_drive) {
        m_hard_drives.erase(std::remove(m_hard_drives.begin(), m_hard_drives.end(), hard_drive), m_hard_drives.end());
    }

    /*!
     * @brief Returns vector of hard drives.
     *
     * @return Reference to vector of hard drives.
     * */
    const std::vector<HardDriveSharedPtr>& get_hard_drives() const {
        return m_hard_drives;
    }

    /*!
     * @brief Find hard drive with given UUID
     * @param[in] uuid Hard drive UUID
     * @return Hard drive with given UUID
     * */
    HardDriveWeakPtr find_hard_drive(const string& uuid) const;

    /*!
     * @brief Returns FRUInfo.
     *
     * @return Reference to FRUInfo.
     * */
    const FruInfo& get_fru_info() const { return m_fru_info; }

    virtual ~StorageController();
};

/* StorageController unique pointer */
typedef std::unique_ptr<StorageController> StorageControllerUniquePtr;

}
}

#endif /* AGENT_FRAMEWORK_MODULE_STORAGE_CONTROLLER_HPP */
