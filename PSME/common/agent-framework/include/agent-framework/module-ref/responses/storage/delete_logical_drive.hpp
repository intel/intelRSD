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
 * @file responses/storage/delete_logical_drive.hpp
 * @brief Definition of storage DeleteLogicalDrive response
 * */

#ifndef AGENT_FRAMEWORK_MODEL_RESPONSES_NETWORK_DELETE_LOGICAL_DRIVE_HPP
#define AGENT_FRAMEWORK_MODEL_RESPONSES_NETWORK_DELETE_LOGICAL_DRIVE_HPP

#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace responses {

using Oem = agent_framework::model::attribute::Oem;

/*! DeleteLogicalDrive response */
class DeleteLogicalDrive  {
public:
    /*!
     * @brief explicit DeleteLogicalDrive response constructor
     */
    explicit DeleteLogicalDrive(Oem oem = Oem{});

    static std::string get_command() {
        return "deleteLogicalDrive";
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create DeleteLogicalDrive from Json
     *
     * @param[in] json the input argument
     *
     * @return new DeleteLogicalDrive
     */
    static DeleteLogicalDrive from_json(const Json::Value& json);

private:
    Oem m_oem{};
};

}
}
}

#endif
/* AGENT_FRAMEWORK_MODEL_RESPONSES_NETWORK_DELETE_LOGICAL_DRIVE_HPP */
