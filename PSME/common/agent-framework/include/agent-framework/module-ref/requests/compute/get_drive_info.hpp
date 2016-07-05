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
 * @file requesrs/compute/get_drive_info.hpp
 * @brief Compute GetDriveInfo request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DRIVE_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DRIVE_INFO_HPP

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetDriveInfo request */
class GetDriveInfo {
public:
    explicit GetDriveInfo(const std::string& drive);

    static std::string get_command() {
        return "getDriveInfo";
    }

    /*!
     * @brief Get component UUID from request
     * @return UUID of component.
     * */
    const std::string& get_uuid() const {
        return m_drive;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetDriveInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetDriveInfo
     */
    static GetDriveInfo from_json(const Json::Value& json);

private:
    std::string m_drive{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DRIVE_INFO_HPP */
