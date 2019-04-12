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
 * @file validation/compute.hpp
 * @brief Methods to validate JSON requests
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace attribute {
/* Forward declaration */
class Attributes;
}
}
}

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

class ComputeValidator {
public:

    /*!
     * @brief Validates SetComponentAttributes method for NetworkInterface resource.
     * @param[in] attributes Attributes deserialized from JSON request.
     * @throw Throws exception if parameter is invalid.
     * */
    static void validate_set_network_interface_attributes(
        const agent_framework::model::attribute::Attributes& attributes);


    /*!
     * @brief Validates SetComponentAttributes method for StorageSubsystem resource.
     * @param[in] attributes Attributes deserialized from JSON request.
     * @throw Throws exception if parameter is invalid.
     * */
    static void validate_set_storage_subsystem_attributes(
        const agent_framework::model::attribute::Attributes& attributes);


    /*!
     * @brief Validates SetComponentAttributes method for NetworkDeviceFunction resource.
     * @param[in] attributes Attributes deserialized from JSON request.
     * @throw Throws exception if parameter is invalid.
     * */
    static void validate_set_network_device_function_attributes(
        const agent_framework::model::attribute::Attributes& attributes);


    /*!
     * @brief Validates SetComponentAttributes method for Trusted module resource.
     * @param[in] attributes Attributes deserialized from JSON request.
     * @throw Throws exception if any parameter is invalid.
     * */
    static void validate_set_trusted_module_attributes(
        const agent_framework::model::attribute::Attributes& attributes);

};

}
}
}
}
