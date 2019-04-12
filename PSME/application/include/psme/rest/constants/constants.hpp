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

#pragma once



#include "psme/rest/constants/chassis.hpp"
#include "psme/rest/constants/ethernet.hpp"
#include "psme/rest/constants/storage.hpp"
#include "psme/rest/constants/system.hpp"
#include "psme/rest/constants/fabric.hpp"
#include "psme/rest/constants/routes.hpp"
#include "psme/rest/constants/telemetry.hpp"


namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for Metadata endpoint.
 */
namespace Metadata {
extern const char METADATA_ROOT_FILE[];
}

/*!
 * @brief Constant literals for use with PathBuilder function.
 */
namespace PathParam {
extern const char REDFISH[];
extern const char METADATA_ROOT[];
extern const char METADATA[];
extern const char BASE_URL[];
extern const char OEM_INTEL_RACKSCALE[];
extern const char METADATA_FILE[];
extern const char SUBSCRIPTION_ID[];
extern const char CHASSIS_ID[];
extern const char SYSTEM_ID[];
extern const char ADAPTER_ID[];
extern const char DEVICE_ID[];
extern const char SERVICE_ID[];
extern const char STORAGE_POOL_ID[];
extern const char CAPACITY_SOURCE_ID[];
extern const char VOLUME_ID[];
extern const char DRIVE_ID[];
extern const char MANAGER_ID[];
extern const char NIC_ID[];
extern const char NETWORK_INTERFACE_ID[];
extern const char NETWORK_DEVICE_FUNCTION_ID[];
extern const char NODE_ID[];
extern const char ETHERNET_SWITCH_ID[];
extern const char SWITCH_PORT_ID[];
extern const char VLAN_ID[];
extern const char PROCESSOR_ID[];
extern const char SIMPLE_STORAGE_ID[];
extern const char MEMORY_ID[];
extern const char FABRIC_ID[];
extern const char ENDPOINT_ID[];
extern const char SWITCH_ID[];
extern const char FUNCTION_ID[];
extern const char ZONE_ID[];
extern const char PORT_ID[];
extern const char FUNCTIONS[];
extern const char RULE_ID[];
extern const char STORAGE_ID[];
extern const char ACL_ID[];
extern const char STATIC_MAC_ID[];
extern const char TASK_ID[];
extern const char ODATA[];
extern const char MESSAGE_REGISTRY_FILE_ID[];
extern const char PSU_ID[];
extern const char METRIC_DEFINITION_ID[];
extern const char METRIC_REPORT_DEFINITION_ID[];
extern const char TRIGGER_ID[];
extern const char SESSION_ID[];
extern const char ACCOUNT_ID[];
extern const char ROLE_ID[];
extern const char INTEL_RACKSCALE_REGISTRY_URL[];


extern const char PATH_SEP;
extern const char VARIABLE_BEGIN;
extern const char VARIABLE_END;
extern const char REGEX_MARK;
extern const char ID_REGEX[];
extern const char XML_REGEX[];
extern const char STRING_ID_REGEX[];
}

/*!
 * @brief Common constant literals for rest endpoints.
 */
namespace Common {
extern const char INTEL[];
extern const char ODATA_CONTEXT[];
extern const char ODATA_ID[];
extern const char ODATA_TYPE[];
extern const char ID[];
extern const char MEMBER_ID[];
extern const char NAME[];
extern const char LINKS[];
extern const char MANUFACTURER[];
extern const char MODEL[];
extern const char SERIAL_NUMBER[];
extern const char PART_NUMBER[];
extern const char ASSET_TAG[];
extern const char STATUS[];
extern const char STATE[];
extern const char HEALTH[];
extern const char HEALTH_ROLLUP[];
extern const char ENUM_STATUS[];
extern const char OEM[];
extern const char RACKSCALE[];
extern const char API_VERSION[];
extern const char LOCATION[];
extern const char CONTAINED_BY[];
extern const char MANAGED_BY[];
extern const char DESCRIPTION[];
extern const char VENDOR_ID[];
extern const char DEVICE_ID[];
extern const char FIRMWARE_VERSION[];
extern const char UUID[];
extern const char ACTIONS[];
extern const char HASH[];
extern const char CHASSIS[];
extern const char RESET_TYPE[];
extern const char ALLOWABLE_RESET_TYPES[];
extern const char TARGET[];
extern const char SKU[];
extern const char RELATED_ITEM[];
extern const char MAC_ADDRESS[];
extern const char METRICS[];
extern const char STORAGE_SERVICES[];
extern const char REDUNDANCY[];
extern const char IDENTIFIERS[];
extern const char DURABLE_NAME[];
extern const char DURABLE_NAME_FORMAT[];
extern const char LUN[];
extern const char ADMINISTRATOR[];
extern const char PERFORMANCE_CONFIGURATION[];
extern const char CURRENT_CONFIGURATION_ID[];
extern const char CONFIGURATIONS[];
}

namespace ErrorMessage {
extern const char ERROR[];
extern const char CODE[];
extern const char MESSAGE[];
extern const char EXTENDED_INFO[];
}

/*!
 * @brief Constant literals for error message objects.
 */
namespace MessageObject {
extern const char JSON_POINTER_PREFIX[];
extern const char MESSAGE_ID[];
extern const char MESSAGE[];
extern const char MESSAGE_ARGS[];
extern const char RELATED_PROPERTIES[];
extern const char SEVERITY[];
extern const char RESOLUTION[];
}

/*!
 * @brief Constant literals for collection endpoints.
 */
namespace Collection {
extern const char ODATA_COUNT[];
extern const char MEMBERS[];
}

/*!
 * @brief Constant literals for /redfish/v1 root endpoint.
 */
namespace Root {
extern const char REDFISH_VERSION[];
extern const char EVENT_SERVICE[];
extern const char SESSION_SERVICE[];
extern const char TASKS[];
extern const char SYSTEMS[];
extern const char STORAGE_SERVICES[];
extern const char MANAGERS[];
extern const char ETHERNET_SWITCHES[];
extern const char REGISTRIES[];
extern const char MESSAGE_REGISTRY[];
extern const char FABRICS[];
extern const char TELEMETRY_SERVICE[];
extern const char UPDATE_SERVICE[];
extern const char SERVICE[];
extern const char ACCOUNT_SERVICE[];
}

/*!
 * @brief Constant literals for Redfish endpoint.
 */
namespace Redfish {
extern const char V1[];
}

/*!
 * @brief Constant literals for /redfish/v1/odata endpoint.
 */
namespace OdataServiceDocument {
extern const char VALUE[];
extern const char NAME[];
extern const char KIND[];
extern const char URL[];
}

/*!
 * @brief Constant literals for Manager endpoint.
 */
namespace Manager {
extern const char MANAGER_TYPE[];
extern const char POWER_STATE[];
extern const char DATE_TIME[];
extern const char DATE_TIME_LOCAL_OFFSET[];
extern const char SERVICE_ENTRY_POINT_UUID[];
extern const char GRAPHICAL_CONSOLE[];
extern const char MAX_CONCURRENT_SESSIONS[];
extern const char CONNECT_TYPES_SUPPORTED[];
extern const char ENABLED[];
extern const char SERVICE_ENABLED[];
extern const char SERIAL_CONSOLE[];
extern const char COMMAND_SHELL[];
extern const char MANAGER_FOR_CHASSIS[];
extern const char MANAGER_FOR_SERVERS[];
extern const char MANAGER_FOR_SWITCHES[];
extern const char MANAGER_FOR_ETHERNET_SWITCHES[];
extern const char MANAGER_FOR_FABRICS[];
extern const char MANAGER_IN_CHASSIS[];
extern const char MANAGER_FOR_SERVICES[];
extern const char NETWORK_PROTOCOL[];
extern const char ETHERNET_INTERFACES[];
extern const char HASH_MANAGER_RESET[];
extern const char MANAGER_RESET[];
extern const char HASH_LOAD_FACTORY_DEFAULTS_ACTION[];
extern const char LOAD_FACTORY_DEFAULTS_ACTION[];
}

/*!
 * @brief Constant literals for NetworkProtocol endpoint.
 */
namespace NetworkProtocol {
extern const char HOST_NAME[];
extern const char FQDN[];
extern const char HTTP[];
extern const char HTTPS[];
extern const char SNMP[];
extern const char VIRTUAL_MEDIA[];
extern const char TELNET[];
extern const char SSDP[];
extern const char IPMI[];
extern const char SSH[];
extern const char KVMIP[];
extern const char PROTOCOL_ENABLED[];
extern const char PORT[];
extern const char NOTIFY_IPV6_SCOPE[];
extern const char NOTIFY_MULTICAST_INTERVAL[];
extern const char NOTIFY_TTL[];
}

/*!
 * @brief Constant IpAddress literals for rest endpoints.
 */
namespace IpAddress {
extern const char ADDRESS[];
extern const char ADDRESS_ORIGIN[];
extern const char GATEWAY[];
extern const char HOST_NAME[];
extern const char FQDN[];
extern const char SUBNET_MASK[];
extern const char PREFIX_LENGTH[];
extern const char SCOPE[];
extern const char ADDRESS_STATE[];
}

/*!
 * @brief Constant literals for NetworkInterface endpoint.
 */
namespace NetworkInterface {
extern const char PERMANENT_MAC_ADDRESS[];
extern const char SPEED_MBPS[];
extern const char AUTO_NEG[];
extern const char FULL_DUPLEX[];
extern const char MTU_SIZE[];
extern const char HOST_NAME[];
extern const char FQDN[];
extern const char MAX_IPv6_STATIC_ADDRESSES[];
extern const char IPv4_ADDRESSES[];
extern const char IPv6_ADDRESS_POLICY_TABLE[];
extern const char PREFIX[];
extern const char PRECEDENCE[];
extern const char LABEL[];
extern const char IPv6_STATIC_ADDRESSES[];
extern const char IPv6_ADDRESSES[];
extern const char IPv6_DEFAULT_GATEWAY[];
extern const char NAME_SERVERS[];
extern const char INTERFACE_ENABLED[];
extern const char UEFI_DEVICE_PATH[];
extern const char VLANS[];
extern const char VLAN[];
extern const char NEIGHBOR_PORT[];
extern const char SUPPORTED_PROTOCOLS[];
extern const char LINK_STATUS[];
}

/*!
 * @brief Constant literals for ActionInfo endpoint.
 */
namespace ActionInfo {
extern const char REDFISH_ACTION_INFO[];
extern const char PARAMETERS[];
extern const char REQUIRED[];
extern const char DATA_TYPE[];
extern const char ALLOWABLE_VALUES[];
}

/*!
 * @brief Constant literals for UpdateService endpoint.
 */
namespace UpdateService {
extern const char SERVICE_ENABLED[];
extern const char HTTP_PUSH_URI[];
extern const char HASH_UPDATE_SERVICE_SIMPLE_UPDATE[];
extern const char SIMPLE_UPDATE[];
extern const char SIMPLE_UPDATE_ACTION_INFO[];
extern const char IMAGE_URI[];
extern const char TRANSFER_PROTOCOL[];
extern const char TARGETS[];
}

/*!
 * @brief Constant literals for EventService endpoint.
 */
namespace EventService {
extern const char SERVICE_ENABLED[];
extern const char DELIVERY_RETRY_ATTEMPTS[];
extern const char DELIVERY_RETRY_INTERVAL_SECONDS[];
extern const char EVENT_TYPES_FOR_SUBSCRIPTION[];
extern const char SUBSCRIPTIONS[];
}

/*!
 * @brief Constant literals for EventSubscription endpoint.
 */
namespace EventSubscription {
extern const char DESTINATION[];
extern const char CONTEXT[];
extern const char PROTOCOL[];
extern const char EVENT_TYPES[];
extern const char ORIGIN_RESOURCES[];
}

/*!
 * @brief Constant literals for EventArray class representing Redfish Event.
 */
namespace EventArray {
extern const char CONTEXT[];
extern const char EVENTS[];
}

/*!
 * @brief Constant literals for Event class representing Redfish EventRecord.
 */
namespace Event {
extern const char EVENT_TYPE[];
extern const char EVENT_ID[];
extern const char EVENT_TIMESTAMP[];
extern const char SEVERITY[];
extern const char MESSAGE[];
extern const char MESSAGE_ID[];
extern const char MESSAGE_ARGS[];
extern const char CONTEXT[];
extern const char ORIGIN_OF_CONDITION[];
}

/*!
 * @brief Constant literals for capabilities.
 */
namespace Capability {
extern const char CHASSIS[];
extern const char COMPUTE[];
extern const char NETWORK[];
extern const char STORAGE[];
extern const char PNC[];
extern const char RMM[];
extern const char NVME[];
extern const char FPGAOF[];
}

/*!
 * @brief Constant literals for TaskService endpoint.
 */
namespace TaskService {
extern const char TASK_SERVICE[];
extern const char SERVICE_ENABLED[];
extern const char DATETIME[];
extern const char COMPLETED_TASK_OVERWRITE_POLICY[];
extern const char LIFE_CYCLE_EVENT_ON_TASK_STATE_CHANGE[];
extern const char TASKS[];
}

/*!
 * @brief Constant literals for Task endpoint.
 * */
namespace Task {
extern const char TASK_STATE[];
extern const char START_TIME[];
extern const char END_TIME[];
extern const char TASK_STATUS[];
extern const char MESSAGES[];
}

namespace Monitor {
extern const char MONITOR[];
}

/*!
 * @brief Constant literals for MessageRegistryFile endpoint
 * */
namespace MessageRegistryFile {
extern const char LANGUAGES[];
extern const char REGISTRY[];
extern const char LOCATION[];
extern const char LANGUAGE[];
extern const char URI[];
extern const char ARCHIVE_URI[];
extern const char PUBLICATION_URI[];
extern const char ARCHIVE_FILE[];
}

/*!
 * @brief Constant literals for MessageRegistry endpoint
 * */
namespace MessageRegistry {
extern const char LANGUAGE[];
extern const char MESSAGES[];
extern const char OWNING_ENTITY[];
extern const char REGISTRY_PREFIX[];
extern const char REGISTRY_VERSION[];
}

/*!
 * @brief Constant literals for SessionService endpoint
 * */
namespace SessionService {
extern const char SERVICE_ENABLED[];
extern const char SESSION_TIMEOUT[];
extern const char SESSIONS[];
}

/*!
 * @brief Constant literals for EventSubscription endpoint.
 * */
namespace Session {
extern const char USER_NAME[];
extern const char PASSWORD[];
extern const char OEM[];
}

/*!
 * @brief Constant literals for AccountService endpoint.
 * */
namespace AccountService {
extern const char ACCOUNT_SERVICE[];
extern const char SERVICE_ENABLED[];
extern const char AUTH_FAILURE_LOGGING_THRESHOLD[];
extern const char MIN_PASSWORD_LENGTH[];
extern const char LOCAL_ACCOUNT_AUTH[];
extern const char ACCOUNTS[];
extern const char ROLES[];
}

/*!
 * @brief Constant literals for Account endpoint.
 * */
namespace Account {
extern const char ENABLED[];
extern const char PASSWORD[];
extern const char USER_NAME[];
extern const char ROLE_ID[];
extern const char LOCKED[];
extern const char ROLE[];
}

/*!
 * @brief Constant literals for Role endpoint.
 * */
namespace Role {
extern const char IS_PREDEFINED[];
extern const char ASSIGNED_PRIVILEGES[];
extern const char OEM_PRIVILEGES[];
extern const char ROLE_ID[];
}

}
}
}
