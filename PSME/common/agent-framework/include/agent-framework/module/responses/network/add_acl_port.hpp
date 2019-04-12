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
 * @file responses/network/add_acl_port.hpp
 * @brief Definition of network AddAclPort response
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! AddAclPort response */
class AddAclPort  {
public:
    using Oem = agent_framework::model::attribute::Oem;

    /*!
     * @brief explicit AddAclPort response constructor
     *
     * @param[in] oem OEM specific data
     * */
    explicit AddAclPort(Oem oem = Oem{});

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     * */
    json::Json to_json() const;

    /*!
     * @brief create AddAclPort from Json
     *
     * @param[in] json the input argument
     *
     * @return new AddAclPort
     * */
    static AddAclPort from_json(const json::Json& json);

private:
    Oem m_oem{};
};

}
}
}
