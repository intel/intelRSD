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
 * */

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/requests/validation/compute.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"



using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;
namespace regex = agent_framework::model::literals::regex;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {


void ComputeValidator::validate_set_network_interface_attributes(const Attributes& attributes) {
    const auto& attribute_names = attributes.get_names();
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        if (literals::NetworkInterface::FRAME_SIZE == name ||
            literals::NetworkInterface::SPEED == name) {
            check_number(value, name, "compute-agent");
        }
        else if (literals::NetworkInterface::AUTO_SENSE == name) {
            check_boolean(value, name, "compute-agent");
        }
        else if (literals::NetworkInterface::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "compute-agent", "Unrecognized attribute.", name, value);
        }
    }
}


void ComputeValidator::validate_set_storage_subsystem_attributes(const Attributes& attributes) {
    const auto& attribute_names = attributes.get_names();
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        if (literals::StorageSubsystem::ENCRYPTION_KEY == name ||
            literals::StorageSubsystem::ASSET_TAG == name) {
            check_string(value, name, "compute-agent");
        }
        else if (literals::StorageSubsystem::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "compute-agent", "Unrecognized attribute.", name, value);
        }
    }
}

namespace {

class EthernetSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::NetworkDeviceFunction::MAC_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::EthernetInterface::MAC_ADDRESS))),
            nullptr
        };
        return procedure;
    }
};

class IscsiBootSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static constexpr const uint32_t MIN_VLAN_ID{1};
        static constexpr const uint32_t MAX_VLAN_ID{4094};
        static constexpr const uint32_t MAX_TCP_PORT{65535};
        static constexpr const uint32_t MAX_LUN{65535};

        static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            literals::IscsiBoot::IP_ADDRESS_TYPE, VALID_OPTIONAL(VALID_ENUM(enums::IPAddressType)),
            literals::IscsiBoot::INITIATOR_IP_ADDRESS, VALID_OPTIONAL(VALID_REGEX(regex::IPAddresses::ADDRESS)),
            literals::IscsiBoot::INITIATOR_NAME, VALID_OPTIONAL(VALID_REGEX(regex::RemoteTarget::INITIATOR_IQN)),
            literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::IPAddresses::ADDRESS))),
            literals::IscsiBoot::INITIATOR_NETMASK, VALID_OPTIONAL(VALID_REGEX(regex::IPAddresses::ADDRESS)),
            literals::IscsiBoot::TARGET_INFO_VIA_DHCP, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
            literals::IscsiBoot::PRIMARY_TARGET_NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::RemoteTarget::TARGET_IQN))),
            literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::IPAddresses::ADDRESS))),
            literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQLT(UINT32, MAX_TCP_PORT))),
            literals::IscsiBoot::PRIMARY_LUN, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQLT(UINT32, MAX_LUN))),
            literals::IscsiBoot::PRIMARY_VLAN_ENABLE, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
            literals::IscsiBoot::PRIMARY_VLAN_ID, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_RANGE(UINT32, MIN_VLAN_ID, MAX_VLAN_ID))),
            literals::IscsiBoot::PRIMARY_DNS, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::IPAddresses::ADDRESS))),
            literals::IscsiBoot::SECONDARY_TARGET_NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::RemoteTarget::TARGET_IQN))),
            literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::IPAddresses::ADDRESS))),
            literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQLT(UINT32, MAX_TCP_PORT))),
            literals::IscsiBoot::SECONDARY_LUN, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQLT(UINT32, MAX_LUN))),
            literals::IscsiBoot::SECONDARY_VLAN_ENABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
            literals::IscsiBoot::SECONDARY_VLAN_ID, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_RANGE(UINT32, MIN_VLAN_ID, MAX_VLAN_ID))),
            literals::IscsiBoot::SECONDARY_DNS, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(regex::IPAddresses::ADDRESS))),
            literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
            literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
            literals::IscsiBoot::AUTHENTICATION_METHOD, VALID_OPTIONAL(VALID_ENUM(enums::FunctionAuthenticationMethod)),
            literals::IscsiBoot::CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
            literals::IscsiBoot::CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
            literals::IscsiBoot::MUTUAL_CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
            literals::IscsiBoot::MUTUAL_CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
            nullptr
        };
        return procedure;
    }
};
}

void ComputeValidator::validate_set_network_device_function_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::NetworkDeviceFunction::ETHERNET, VALID_OPTIONAL(VALID_ATTRIBUTE(EthernetSchema)),
        literals::NetworkDeviceFunction::ISCSI_BOOT, VALID_OPTIONAL(VALID_ATTRIBUTE(IscsiBootSchema)),
        literals::NetworkDeviceFunction::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    );
    const auto& json = attributes.to_json();
    validator.validate(json);

    if (json.count(literals::NetworkDeviceFunction::ISCSI_BOOT)) {
        const auto& boot = json[literals::NetworkDeviceFunction::ISCSI_BOOT];

        auto check_length = [&] (const char* attribute, size_t min, size_t max) {
            if (boot.count(attribute) && !boot[attribute].is_null()) {
                size_t length = boot[attribute].get<std::string>().size();
                if (length < min || length > max) {
                    std::string error_message = std::string{"String must be between "} + std::to_string(min)
                                                + " and " + std::to_string(max) + " characters long.";
                    THROW(GamiException, "agent-framework",  ErrorCode::INVALID_VALUE_FORMAT, error_message,
                        InvalidField::create_json_data_from_field(
                              std::string(literals::NetworkDeviceFunction::ISCSI_BOOT) + "/" + attribute, boot[attribute]));
                }
            }
        };

        check_length(literals::IscsiBoot::CHAP_USERNAME, 1, 32);
        check_length(literals::IscsiBoot::CHAP_SECRET, 12, 16);
        check_length(literals::IscsiBoot::MUTUAL_CHAP_USERNAME, 1, 32);
        check_length(literals::IscsiBoot::MUTUAL_CHAP_SECRET, 12, 16);
    }

    log_debug("compute-agent", "Request validation passed.");
}


void ComputeValidator::validate_set_trusted_module_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::TrustedModule::CLEAR_OWNERSHIP, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        literals::TrustedModule::DEVICE_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        literals::TrustedModule::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr);
    validator.validate(attributes.to_json());

    log_debug("agent-framework", "Request validation passed.");
}

}
}
}
}
