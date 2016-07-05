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
 * @file requests/compute/get_processor_info.hpp
 * @brief Compute GetProcessorInfo request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_PROCESSOR_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_PROCESSOR_INFO_HPP

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetProcessorInfo request*/
class GetProcessorInfo {
public:
    explicit GetProcessorInfo(const std::string& processor);

    static std::string get_command() {
        return "getProcessorInfo";
    }

   /*!
    * @brief Get processor id from request
    * @return Computer system id string (UUID)
    * */
   const std::string& get_uuid() const {
       return m_processor;
   }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetProcessorInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetProcessorInfo
     */
    static GetProcessorInfo from_json(const Json::Value& json);

private:
   std::string m_processor{};

};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_PROCESSOR_INFO_HPP */
