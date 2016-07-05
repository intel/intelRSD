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
 * @file command/common/get_chassis_info.hpp
 * @brief Generic common GetChassisInfo command
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMMON_GET_CHASSIS_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMMON_GET_CHASSIS_INFO_HPP


#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetChassisInfo request */
class GetChassisInfo {
public:
    explicit GetChassisInfo(const std::string& chassis);

    static std::string get_command() {
        return "getChassisInfo";
    }

    /*!
     * @brief Get chassis UUID from request
     * @return uuid string
     * */
    const std::string& get_uuid() const {
        return m_chassis;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetChassisInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetChassisInfo
     */
    static GetChassisInfo from_json(const Json::Value& json);

private:
    std::string m_chassis{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_COMMON_GET_CHASSIS_INFO_HPP */
