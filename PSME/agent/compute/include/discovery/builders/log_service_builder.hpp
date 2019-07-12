/*!
 * @brief Log service builder class interface.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file log_service_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/log_service.hpp"



namespace agent {
namespace compute {
namespace discovery {

class LogServiceBuilder {
public:
    /*!
     * @brief Build SEL type log service object.
     * @param parent_uuid Parent UUID.
     * @return Built log service object for SEL.
     */
    static agent_framework::model::LogService build_sel_log(const std::string& parent_uuid);
};

}
}
}
