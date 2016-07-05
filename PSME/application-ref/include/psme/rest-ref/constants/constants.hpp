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
 * */

#pragma once
#include "psme/rest-ref/constants/ethernet.hpp"
#include "psme/rest-ref/constants/storage.hpp"
#include "psme/rest-ref/constants/system.hpp"

namespace app {
namespace rest {
namespace constants {

namespace Metadata {
    extern const char METADATA_ROOT_FILE[];
}

namespace PathParam {
    extern const char REDFISH[];
    extern const char METADATA_ROOT[];
    extern const char METADATA[];
    extern const char BASE_URL[];
    extern const char METADATA_FILE[];
    extern const char SUBSCRIPTION_ID[];
    extern const char CHASSIS_ID[];
    extern const char POWER_ZONE_ID[];
    extern const char SYSTEM_ID[];
    extern const char ADAPTER_ID[];
    extern const char DEVICE_ID[];
    extern const char SERVICE_ID[];
    extern const char DRIVE_ID[];
    extern const char LOGICAL_DRIVE_ID[];
    extern const char REMOTE_TARGET_ID[];
    extern const char MANAGER_ID[];
    extern const char NIC_ID[];
    extern const char NODE_ID[];
    extern const char ETHERNET_SWITCH_ID[];
    extern const char SWITCH_PORT_ID[];
    extern const char VLAN_ID[];
    extern const char PROCESSOR_ID[];
    extern const char MEMORY_CHUNK_ID[];
    extern const char SIMPLE_STORAGE_ID[];
    extern const char DIMM_CONFIG_ID[];
    extern const char ACL_ID[];
    extern const char RULE_ID[];
    extern const char STATIC_MAC_ID[];

    extern const char PATH_SEP;
    extern const char VARIABLE_BEGIN;
    extern const char VARIABLE_END;
    extern const char REGEX_MARK;
    extern const char ID_REGEX[];
    extern const char XML_REGEX[];
}

namespace Common {
    extern const char INTEL[];
    extern const char ODATA_CONTEXT[];
    extern const char ODATA_ID[];
    extern const char ODATA_TYPE[];
    extern const char ID[];
    extern const char NAME[];
    extern const char LINKS[];
    extern const char MANUFACTURER[];
    extern const char MODEL[];
    extern const char SERIAL[];
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
}

namespace Collection {
    extern const char ODATA_COUNT[];
    extern const char ODATA_ID[];
    extern const char MEMBERS[];
}

namespace Root {
    extern const char REDFISH_VERSION[];
    extern const char EVENT_SERVICE[];
    extern const char CHASSIS[];
    extern const char SYSTEMS[];
    extern const char SERVICES[];
    extern const char MANAGERS[];
    extern const char ETHERNET_SWITCHES[];
}


namespace Redfish {
     extern const char V1[];
}

namespace Chassis {
    extern const char CHASSIS_TYPE[];
    extern const char SKU[];
    extern const char INDICATOR_LED[];
    extern const char LOCATION[];
    extern const char PARENT_ID[];
    extern const char CONTAINS[];
    extern const char COMPUTER_SYSTEMS[];
    extern const char SWITCHES[];
    extern const char MANAGERS_IN[];
    extern const char THERMAL_ZONES[];
    extern const char POWER_ZONES[];
}

namespace Zone {
    extern const char RACK_LOCATION[];
    extern const char UHEIGHT[];
    extern const char ULOCATION[];
    extern const char UNIT[];
    extern const char XLOCATION[];
    extern const char MEMBER_ID[];
    extern const char RELATED_ITEM[];
}

namespace PowerZone {
    extern const char POWER_CONTROL[];
    extern const char MAX_PSUS_SUPPORTED[];
    extern const char POWER_AVAILABLE[];
    extern const char POWER_CAPACITY[];
    extern const char POWER_CONSUMED[];
    extern const char POWER_SUPPLIES[];
    extern const char LAST_POWER_OUTPUT[];
}

namespace ThermalZone {
    extern const char FANS[];
    extern const char FAN_NAME[];
    extern const char PHYSICAL_CONTEXT[];
    extern const char READING[];
    extern const char READING_UNITS[];
    extern const char DESIRED_SPEED_RPM[];
    extern const char MAX_FANS_SUPPORTED[];
    extern const char PWM[];
    extern const char POLICY[];
    extern const char TEMPERATURES[];
    extern const char READING_CELCIUS[];
    extern const char SENSOR_NUMBER[];
}

namespace Manager {
    extern const char TARGET[];
    extern const char MANAGER_TYPE[];
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
    extern const char MANAGER_LOCATION[];
    extern const char MANAGER_FOR_SERVICES[];
    extern const char MANAGER_FOR_REMOTE_TARGETS[];
    extern const char MANAGER_FOR_LOGICAL_DRIVES[];
    extern const char MANAGER_FOR_PHYSICAL_DRIVES[];
    extern const char NETWORK_PROTOCOL[];
    extern const char ETHERNET_INTERFACES[];
}

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

namespace NetworkInterface {
    extern const char MAC_ADDRESS[];
    extern const char PERMANENT_MAC_ADDRESS[];
    extern const char SPEED[];
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
    extern const char NEIGHBOR_PORT[];
}

namespace EventService {
    extern const char SERVICE_ENABLED[];
    extern const char DELIVERY_RETRY_ATTEMPTS[];
    extern const char DELIVERY_RETRY_INTERVAL_SECONDS[];
    extern const char EVENT_TYPES_FOR_SUBSCRIPTION[];
    extern const char STATUS_CHANGE[];
    extern const char RESOURCE_UPDATED[];
    extern const char RESOURCE_ADDED[];
    extern const char RESOURCE_REMOVED[];
    extern const char ALERT[];
    extern const char SUBSCRIPTIONS[];
    extern const char ACTIONS[];
    extern const char SEND_TEST_EVENT[];
    extern const char TARGET[];
    extern const char ALLOWABLE_EVENT_TYPES[];
}

namespace EventSubscription {
    extern const char DESTINATION[];
    extern const char CONTEXT[];
    extern const char PROTOCOL[];
    extern const char EVENT_TYPES[];
}

namespace Capability {
    extern const char CHASSIS[];
    extern const char COMPUTE[];
    extern const char NETWORK[];
    extern const char STORAGE[];
}

}
}
}

