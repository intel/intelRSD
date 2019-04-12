/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file responses/network/add_acl.hpp
 * @brief Definition of network AddAcl response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddAcl request */
class AddAcl  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddAcl response constructor
     *
     * @param[in] acl ACL identifier
     * @param[in] oem OEM specific data
     * */
    explicit AddAcl(const std::string& acl = std::string{},
                    const Oem& oem = Oem{});

    /*!
     * @brief Get ACL UUID from response
     *
     * @return acl std::string
     * */
    const std::string& get_acl() const {
        return m_acl;
    }

    /*!
     * @brief set ACL UUID
     *
     * @param acl acl std::string
     * */
    void set_acl(const std::string& acl) {
        m_acl = acl;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create AddAcl from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddAcl
     * */
    static AddAcl from_json(const json::Json& json);

private:
    std::string m_acl{};
    Oem m_oem{};
};

}
}
}
