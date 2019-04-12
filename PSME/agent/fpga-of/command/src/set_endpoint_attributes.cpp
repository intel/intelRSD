/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_endpoint_attributes.cpp
 * @brief Implementation of SetEndpointAttributes command
 * */

#include "agent-framework/module/requests/validation/common.hpp"
#include "command/set_endpoint_attributes.hpp"



using namespace agent_framework::model::requests::validation;
using namespace agent_framework::model::attribute;


void agent::fpgaof::command::set_endpoint_attributes(AgentContext::SPtr,
                                                   const Uuid&,
                                                   const Attributes& attributes,
                                                   SetComponentAttributes::Response& response) {

    CommonValidator::validate_set_endpoint_attributes(attributes);
    if (attributes.empty()) {
        log_info("fpgaof-agent", "Nothing has been changed (empty request)");
        return;
    }

    const auto& attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& value = attributes.get_value(attribute_name);

        try {
            log_debug("fpgaof-agent", "Attribute name: " << attribute_name);
            log_debug("fpgaof-agent", "Attribute value: " << value.dump());

            THROW(agent_framework::exceptions::UnsupportedField, "fpgaof-agent",
                  "Setting attribute is not supported.", attribute_name, value);
        }
        catch (const agent_framework::exceptions::GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}
