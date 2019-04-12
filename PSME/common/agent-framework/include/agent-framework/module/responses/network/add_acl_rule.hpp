/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file responses/network/add_acl_rule.hpp
 * @brief Definition of network AddAclRule response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddAclRule request */
class AddAclRule  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddAclRule response constructor
     *
     * @param[in] rule Rule identifier
     * @param[in] oem OEM specific data
     * */
    explicit AddAclRule(const std::string& rule = {},
                        Oem oem = Oem{});

    /*!
     * @brief Set rule UUID
     *
     * @param[in] rule Rule UUID
     * */
    void set_rule(const std::string& rule) {
        m_rule = rule;
    }

    /*!
     * @brief Get rule UUID from response
     *
     * @return rule std::string
     * */
    const std::string& get_rule() const {
        return m_rule;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create AddAclRule from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddAclRule
     * */
    static AddAclRule from_json(const json::Json& json);

private:
    std::string m_rule{};
    Oem m_oem{};
};

}
}
}
