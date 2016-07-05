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
 * @file responses/storage/add_iscsi_target.hpp
 * @brief Definition of storage AddIscsiTarget response
 * */

#ifndef AGENT_FRAMEWORK_MODEL_RESPONSES_STORAGE_ADD_ISCSI_TARGET_HPP
#define AGENT_FRAMEWORK_MODEL_RESPONSES_STORAGE_ADD_ISCSI_TARGET_HPP

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace responses {

using Oem = agent_framework::model::attribute::Oem;

/*! AddIscsiTarget response */
class AddIscsiTarget  {
public:
    /*!
     * @brief explicit AddIscsiTarget response constructor
     */
    explicit AddIscsiTarget(const std::string& target,
                            Oem oem = Oem{});

    static std::string get_command() {
        return "addiSCSITarget";
    }

    /*!
     * @brief Sets iSCSI target UUID
     * @param[in] target Target UUID
     */
    void set_target(const std::string& target) {
        m_target = target;
    }

    /*!
     * @brief Gets iSCSI target UUID
     * @return iSCSI target UUID
     */
    const std::string& get_target() const {
        return m_target;
    }

    /*!
     * @brief Transform request to JSON
     *
     * @return created JSON value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddIscsiTarget from JSON
     *
     * @param[in] json The input argument
     *
     * @return new AddIscsiTarget
     */
    static AddIscsiTarget from_json(const Json::Value& json);

private:
    Oem m_oem{};
    std::string m_target{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_RESPONSES_STORAGE_ADD_ISCSI_TARGET_HPP */
