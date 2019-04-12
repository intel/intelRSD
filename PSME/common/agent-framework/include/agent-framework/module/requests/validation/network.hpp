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
         * @brief Validates SetComponentAttributes method for Switch resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_switch_attributes(
            const agent_framework::model::attribute::Attributes& attributes);

        /*!
         * @brief Validates SetComponentAttributes method for Port resource.
         * @param[in] attributes Attributes deserialized from JSON request.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_set_port_attributes(
            const EthernetSwitchPort& port,
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

        /*!
         * @brief Validates priorities list if it contains duplicated priorities to set.
         * @param[in] priorities list of priorities.
         * @throw Throws exception if parameter is invalid.
         * */
        static void validate_request_switch_port_priorities(
                const agent_framework::model::attribute::Array<std::uint32_t>& priorities);

        /*!
         * @brief Validate QoS Application Protocol configuration on a switch.
         * @param[in] application_mappings Application Protocol configuration to be set
         * @throw Throws exception if parameter is invalid.
         */
        static void validate_request_switch_qos_application_protocol(
                const agent_framework::model::attribute::Array<
                        agent_framework::model::attribute::QosApplicationProtocol>& application_mappings);

        /*!
         * @brief Validate QoS Priority to Priority Group mapping on a switch.
         * @param[in] priority_mappings Priority mapping to be set
         * @throw Throws exception if parameter is invalid.
         */
        static void validate_request_switch_qos_priority_group_mapping(
                const agent_framework::model::attribute::Array<
                        agent_framework::model::attribute::QosPriorityGroupMapping>& priority_mappings);

        /*!
         * @brief Validate QoS Bandwidth Allocation on a switch.
         * @param[in] bandwidth_allocations Bandwidth Allocation configuration to be set
         * @throw Throws exception if parameter is invalid.
         */
        static void validate_request_switch_qos_bandwidth_allocation(
                const agent_framework::model::attribute::Array<
                        agent_framework::model::attribute::QosBandwidthAllocation>& bandwidth_allocations);
    };
}
}
}
}

