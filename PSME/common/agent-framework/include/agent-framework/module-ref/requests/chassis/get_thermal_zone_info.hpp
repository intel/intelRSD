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
 * @file command/chassis/get_thermal_zone_info.hpp
 * @brief Generic chassis GetThermalZoneInfo command
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_GET_THERMAL_ZONE_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_GET_THERMAL_ZONE_INFO_HPP


#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetThermalZoneInfo request */
class GetThermalZoneInfo {
public:
    explicit GetThermalZoneInfo(const std::string& zone);

    static std::string get_command() {
        return "getThermalZoneInfo";
    }

    /*!
     * @brief Get thermal zone UUID from request
     * @return uuid string
     * */
    const std::string& get_thermal_zone() const {
        return m_zone;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetThermalZoneInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetThermalZoneInfo
     */
    static GetThermalZoneInfo from_json(const Json::Value& json);

private:
    std::string m_zone{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_GET_THERMAL_ZONE_INFO_HPP */
