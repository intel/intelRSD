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
 * @file requests/storage/add_logical_drive.hpp
 * @brief Storage AddLogicalDrive request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_LOGICAL_DRIVE_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_LOGICAL_DRIVE_HPP

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include <vector>
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! AddLogicalDrive request */
class AddLogicalDrive{
public:
    explicit AddLogicalDrive(
        const std::string& type,
        double capacityGB,
        const std::string& mode,
        const std::string& master,
        const OptionalField<std::string>& image,
        bool snapshot,
        bool mark_as_protected,
        const std::vector<std::string>& drives,
        agent_framework::model::attribute::Oem oem
    );

    static std::string get_command() {
        return "addLogicalDrive";
    }

    /*!
     * @brief Get logical drive type
     * @return Logical drive type
     * */
    std::string get_type() const {
        return m_type;
    }

    /*!
     * @brief Get logical drive capacity
     * @return Logical drive capacity
     * */
    double get_capacity_gb() const {
        return m_capacityGB;
    }

    /*!
     * @brief Get logical drive capacity in bytes
     * @return Logical drive capacity in bytes
     * */
    uint64_t get_capacity_bytes() const {
        return static_cast<uint64_t>(m_capacityGB * 1024 * 1024 * 1024);
    }

    /*!
     * @brief Get logical drive creation mode
     * @return Logical drive mode
     * */
    std::string get_mode() const {
        return m_mode;
    }

    /*!
     * @brief Get logical drive master UUID
     * @return Logical drive master UUID
     * */
    std::string get_master() const {
        return m_master;
    }

    /*!
     * @brief Get logical drive image
     * @return Logical drive image
     * */
    const OptionalField<std::string>& get_image() const { return m_image; }

    /*!
    * @brief Set logical_drive image info
    * @param[in] logical_drive_image LogicalDrive image info
    * */
   void set_image(const OptionalField<std::string>& logical_drive_image) {
       m_image = logical_drive_image;
   }

    /*!
     * @brief Indicates if the logical drive should be marked
     * as write/delete protected after creation.
     * @return true if protected
     * */
    bool is_protected() const {
        return m_protected;
    }

    /*!
     * @brief The UUIDs of the drives (physical or logical)
     * that should be used to create the new logical drive on top
     *
     * @return Reference to vector of drives.
     * */
    std::vector<std::string>& get_drives() {
        return m_drives;
    }

    /*!
     * Get OEM specific data
     *
     * @return LogicalDrive OEM specific data
     * */
    agent_framework::model::attribute::Oem& get_oem() {
        return m_oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddLogicalDrive form Json
     *
     * @param[in] json the input argument
     *
     * @return new AddLogicalDrive
     */
    static AddLogicalDrive from_json(const Json::Value& json);

private:
    std::string m_type{};
    OptionalField<std::string> m_image{};
    double m_capacityGB{};
    std::string m_mode{};
    std::string m_master{};
    bool m_snapshot{};
    bool m_protected{};
    std::vector<std::string> m_drives{};
    agent_framework::model::attribute::Oem m_oem{};
};


}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_LOGICAL_DRIVE_HPP */
