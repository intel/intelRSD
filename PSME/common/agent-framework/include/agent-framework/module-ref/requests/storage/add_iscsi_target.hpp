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
 * @file requests/storage/get_target_info.hpp
 * @brief Storage AddISCSITarget request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_ISCSI_TARGET_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_ISCSI_TARGET_HPP

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include "agent-framework/module-ref/model/attributes/array.hpp"
#include "agent-framework/module-ref/model/attributes/target_lun.hpp"

#include <vector>
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! AddIscsiTarget request */
class AddIscsiTarget{
public:
    using TargetLuns = attribute::Array<attribute::TargetLun>;

    explicit AddIscsiTarget(const std::string& initiator_iqn,
                            const std::string& target_iqn,
                            attribute::Oem oem);

    static std::string get_command() {
        return "addiSCSITarget";
    }

    /*!
     * @brief Get target luns
     * @return Target luns
     */
    const TargetLuns& get_target_luns() const {
        return m_target_luns;
    }

    /*!
     * @brief Set target luns
     * @param[in] target_luns Array of Target LUNs
     */
    void set_target_luns(const TargetLuns& target_luns) {
        m_target_luns = target_luns;
    }

    /*!
     * @brief Get target iqn
     * @return Target iqn
     */
    const std::string& get_target_iqn() const {
        return m_target_iqn;
    }

    /*!
     * @brief Get initiator iqn
     * @return Initiator iqn
     */
    const std::string& get_initiator_iqn() const {
        return m_initiator_iqn;
    }

    /*!
     * @brief Get OEM data
     * @return OEM data object
     */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief add Target Lun
     * @param target_lun the Target Lun to add
     */
    void add_target_lun(const attribute::TargetLun& target_lun) {
        m_target_luns.add_entry(target_lun);
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddIscsiTarget form Json
     *
     * @param[in] json the input argument
     *
     * @return new AddIscsiTarget
     */
    static AddIscsiTarget from_json(const Json::Value& json);


private:
    std::string m_initiator_iqn{};
    std::string m_target_iqn{};
    TargetLuns m_target_luns{};
    attribute::Oem m_oem{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_ADD_ISCSI_TARGET_HPP */
