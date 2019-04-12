/*!
 * @brief Contains Intel RackScale registry message
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
 * @file intel_rackscale_registry.hpp
 * */

#pragma once



#include <string>



namespace psme {
namespace rest {
namespace registries {

/*!
 * Make registry message string for Intel RackScale message registry.
 *
 * @return String containing a json object with Intel RackScale registry messages.
 * */
static std::string get_intel_rackscale_registry() {
    static std::string intel_rackscale_registry = R"({
        "IntelRackScaleRegistry": [
            {
                "@odata.type": "#MessageRegistry.v1_0_0.MessageRegistry",
                "Id": "Intel_RackScale.1.0.0",
                "Name": "Intel RackScale Message Registry",
                "Language": "en",
                "Description": "This registry defines messages specific to Intel RackScale",
                "RegistryPrefix": "Intel_RackScale",
                "RegistryVersion":  "1.0.0",
                "OwningEntity": "Intel Corporation",
                "Messages": {
                    "PropertyNotModifiable": {
                        "Description": "Indicates that a property cannot be modified even though the metadata specifies it as writable",
                        "Message": "The service is unable to modify the property %1 even though metadata specifies it as writeable.",
                        "Severity": "Warning",
                        "NumberOfArgs": 1,
                        "ParamTypes": [
                            "string"
                        ],
                        "Resolution": "Remove the unmodifiable property from the request body and resubmit the request."
                    },
                    "PropertyValueRestricted": {
                        "Description": "Indicates that the value given for a property is not within restrictions imposed by the Service (even though it may be correct according to metadata) ",
                        "Message": "The value %1 for property %2 is not within restrictions imposed by the Service.",
                        "Severity": "Warning",
                        "NumberOfArgs": 1,
                        "ParamTypes": [
                            "string",
                            "string"
                        ],
                        "Resolution": "Correct the value for the property in the request body and resubmit the request."
                    }
                }
            }
        ]
    })";

    return intel_rackscale_registry;
}

}
}
}
