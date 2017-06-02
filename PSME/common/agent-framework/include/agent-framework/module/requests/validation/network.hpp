/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file validation/network.hpp
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

    class NetworkValidator {
    public:

        /*!
         * @brief Validates SetComponentAttributes method for Port resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_port_attributes(
            const agent_framework::model::attribute::Attributes& attributes);

        /*!
         * @brief Validates SetComponentAttributes method for AclRule resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_acl_rule_attributes(
            const agent_framework::model::attribute::Attributes& attributes);

        /*!
         * @brief Validates SetComponentAttributes method for Vlan resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_vlan_attributes(
            const agent_framework::model::attribute::Attributes& attributes);

        /*!
         * @brief Validates SetComponentAttributes method for PortVlan resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_port_vlan_attributes(
            const agent_framework::model::attribute::Attributes& attributes);

        /*!
         * @brief Validates SetComponentAttributes method for PortVlan resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_static_mac_attributes(
                const agent_framework::model::attribute::Attributes& attributes);
    };
}
}
}
}

