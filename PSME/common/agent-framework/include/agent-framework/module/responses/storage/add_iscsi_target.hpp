/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddIscsiTarget response */
class AddIscsiTarget  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddIscsiTarget response constructor
     */
    explicit AddIscsiTarget(const std::string& target = std::string{},
                            Oem oem = Oem{});

    static std::string get_command() {
        return literals::Command::ADD_ISCSI_TARGET;
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
    json::Json to_json() const;

    /*!
     * @brief create AddIscsiTarget from JSON
     *
     * @param[in] json The input argument
     *
     * @return new AddIscsiTarget
     */
    static AddIscsiTarget from_json(const json::Json& json);

private:
    Oem m_oem{};
    std::string m_target{};
};

}
}
}
