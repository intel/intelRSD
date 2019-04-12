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
 *
 * @file common.cpp
 * @brief Contains common constants used all over the model
 * */

#include "agent-framework/module/constants/common.hpp"



namespace agent_framework {
namespace model {
namespace literals {

constexpr const char ServiceUuid::UUID[];

constexpr const char Status::STATUS[];
constexpr const char Status::STATE[];
constexpr const char Status::HEALTH[];

constexpr const char FruInfo::FRU_INFO[];
constexpr const char FruInfo::SERIAL[];
constexpr const char FruInfo::MANUFACTURER[];
constexpr const char FruInfo::MODEL[];
constexpr const char FruInfo::PART[];

constexpr const char Collections::COLLECTIONS[];
constexpr const char Collections::NAME[];
constexpr const char Collections::TYPE[];

constexpr const char Collection::COMPONENT[];
constexpr const char Collection::NAME[];

constexpr const char Oem::OEM[];

constexpr const char Component::COMPONENT[];
constexpr const char Component::ATTRIBUTES[];
constexpr const char Component::ATTRIBUTE[];
constexpr const char Component::NOTIFICATION[];
constexpr const char Component::PARENT[];
constexpr const char Component::TYPE[];
constexpr const char Component::TIME_STAMP[];
constexpr const char Component::MESSAGE[];
constexpr const char Component::CODE[];

constexpr const char SubcomponentEntry::SUBCOMPONENT[];

constexpr const char ManagerEntry::MANAGER[];

constexpr const char TaskEntry::TASK[];
constexpr const char TaskEntry::OEM[];

constexpr const char SerialConsole::SERIAL_CONSOLE[];
constexpr const char SerialConsole::SIGNAL_TYPE[];
constexpr const char SerialConsole::BITRATE[];
constexpr const char SerialConsole::PARITY[];
constexpr const char SerialConsole::DATA_BITS[];
constexpr const char SerialConsole::STOP_BITS[];
constexpr const char SerialConsole::FLOW_CONTROL[];
constexpr const char SerialConsole::PIN_OUT[];
constexpr const char SerialConsole::ENABLED[];
constexpr const char SerialConsole::MAX_SESSIONS[];
constexpr const char SerialConsole::TYPES_SUPPORTED[];

constexpr const char ProcessorMemory::TYPE[];
constexpr const char ProcessorMemory::CAPACITY_MB[];
constexpr const char ProcessorMemory::SPEED_MHZ[];

constexpr const char NetworkService::NETWORK_SERVICES[];
constexpr const char NetworkService::NAME[];
constexpr const char NetworkService::PORT[];
constexpr const char NetworkService::ENABLED[];

constexpr const char Manager::MANAGER[];
constexpr const char Manager::OEM[];
constexpr const char Manager::STATUS[];
constexpr const char Manager::TYPE[];
constexpr const char Manager::MODEL[];
constexpr const char Manager::LOCATION[];
constexpr const char Manager::FIRMWARE_VERSION[];
constexpr const char Manager::IPv4_ADDRESS[];
constexpr const char Manager::NETWORK_SERVICES[];
constexpr const char Manager::GRAPHICAL_CONSOLE[];
constexpr const char Manager::SERIAL_CONSOLE[];
constexpr const char Manager::COMMAND_SHELL[];
constexpr const char Manager::COLLECTIONS[];
constexpr const char Manager::PARENT_ID[];
constexpr const char Manager::GUID[];
constexpr const char Manager::DATE_TIME[];
constexpr const char Manager::DATE_TIME_LOCAL_OFFSET[];
constexpr const char Manager::RESET[];
constexpr const char Manager::FACTORY_DEFAULTS[];
constexpr const char Manager::PACKAGE_URL[];
constexpr const char Manager::ALLOWED_ACTIONS[];

constexpr const char NetworkInterface::INTERFACE[];
constexpr const char NetworkInterface::STATUS[];
constexpr const char NetworkInterface::FRAME_SIZE[];
constexpr const char NetworkInterface::SPEED[];
constexpr const char NetworkInterface::AUTO_SENSE[];
constexpr const char NetworkInterface::MAC_ADDRESS[];
constexpr const char NetworkInterface::VLAN_ENABLE[];
constexpr const char NetworkInterface::VLAN_COUNT[];
constexpr const char NetworkInterface::FULL_DUPLEX[];
constexpr const char NetworkInterface::IPv4_ADDRESS[];
constexpr const char NetworkInterface::IPv6_ADDRESS[];
constexpr const char NetworkInterface::IPv4_ADDRESSES[];
constexpr const char NetworkInterface::IPv6_ADDRESSES[];
constexpr const char NetworkInterface::IPv6_DEFAULT_GATEWAY[];
constexpr const char NetworkInterface::MAX_IPv6_STATIC_ADDRESSES[];
constexpr const char NetworkInterface::DEFAULT_VLAN[];
constexpr const char NetworkInterface::SUPPORTED_PROTOCOLS[];
constexpr const char NetworkInterface::FACTORY_MAC_ADDRESS[];
constexpr const char NetworkInterface::COLLECTIONS[];
constexpr const char NetworkInterface::OEM[];

constexpr const char GraphicalConsole::ENABLED[];
constexpr const char GraphicalConsole::MAX_SESSIONS[];
constexpr const char GraphicalConsole::TYPES_SUPPORTED[];

constexpr const char CommandShell::ENABLED[];
constexpr const char CommandShell::MAX_SESSIONS[];
constexpr const char CommandShell::TYPES_SUPPORTED[];

constexpr const char Ipv4Address::IPv4_ADDRESS[];
constexpr const char Ipv4Address::ADDRESS[];
constexpr const char Ipv4Address::SUBNET_MASK[];
constexpr const char Ipv4Address::ADDRESS_ORIGIN[];
constexpr const char Ipv4Address::GATEWAY[];

constexpr const char Ipv6Address::IPv6_ADDRESS[];
constexpr const char Ipv6Address::ADDRESS[];
constexpr const char Ipv6Address::PREFIX_LENGTH[];
constexpr const char Ipv6Address::ADDRESS_ORIGIN[];
constexpr const char Ipv6Address::ADDRESS_STATE[];

constexpr const char Intel::INTEL_CORP[];

constexpr const char Drive::DRIVE[];
constexpr const char Drive::NAME[];
constexpr const char Drive::DESCRIPTION[];
constexpr const char Drive::STATUS[];
constexpr const char Drive::INTERFACE[];
constexpr const char Drive::TYPE[];
constexpr const char Drive::RPM[];
constexpr const char Drive::FIRMWARE_VERSION[];
constexpr const char Drive::LATENCY_TRACKING_ENABLED[];
constexpr const char Drive::CAPACITY[];
constexpr const char Drive::FRU_INFO[];
constexpr const char Drive::INDICATOR_LED[];
constexpr const char Drive::ASSET_TAG[];
constexpr const char Drive::CAPABLE_SPEED_GBS[];
constexpr const char Drive::NEGOTIATED_SPEED_GBS[];
constexpr const char Drive::LOCATION[];
constexpr const char Drive::STATUS_INDICATOR[];
constexpr const char Drive::REVISION[];
constexpr const char Drive::FAILURE_PREDICTED[];
constexpr const char Drive::SKU[];
constexpr const char Drive::IDENTIFIERS[];
constexpr const char Drive::HOTSPARE_TYPE[];
constexpr const char Drive::ENCRYPTION_ABILITY[];
constexpr const char Drive::ENCRYPTION_STATUS[];
constexpr const char Drive::BLOCK_SIZE_BYTES[];
constexpr const char Drive::PREDICTED_MEDIA_LIFE_LEFT[];
constexpr const char Drive::ERASED[];
constexpr const char Drive::COLLECTIONS[];
constexpr const char Drive::OEM[];
constexpr const char Drive::SECURELY_ERASE[];

constexpr const char Location::INFO[];
constexpr const char Location::INFO_FORMAT[];

constexpr const char Identifier::DURABLE_NAME[];
constexpr const char Identifier::DURABLE_NAME_FORMAT[];

constexpr const char Message::MESSAGE_ID[];
constexpr const char Message::MESSAGE_CONTENT[];
constexpr const char Message::RELATED_PROPERTIES[];
constexpr const char Message::MESSAGE_ARGS[];
constexpr const char Message::SEVERITY[];
constexpr const char Message::RESOLUTION[];
constexpr const char Message::OEM[];

constexpr const char Task::TASK[];
constexpr const char Task::STATE[];
constexpr const char Task::START_TIME[];
constexpr const char Task::END_TIME[];
constexpr const char Task::STATUS[];
constexpr const char Task::MESSAGES[];
constexpr const char Task::NAME[];
constexpr const char Task::OEM[];

constexpr const char ConnectedEntity::ROLE[];
constexpr const char ConnectedEntity::ENTITY[];
constexpr const char ConnectedEntity::IDENTIFIERS[];
constexpr const char ConnectedEntity::LUN[];

constexpr const char Transport::PROTOCOL[];
constexpr const char Transport::TYPE[];
constexpr const char Transport::DETAILS[];

constexpr const char IpTransportDetail::IPV4_ADDRESS[];
constexpr const char IpTransportDetail::IPV6_ADDRESS[];
constexpr const char IpTransportDetail::PORT[];
constexpr const char IpTransportDetail::PROTOCOL[];
constexpr const char IpTransportDetail::INTERFACE[];

constexpr const char Metric::METRIC[];
constexpr const char Metric::UUID[];
constexpr const char Metric::NAME[];
constexpr const char Metric::TIMESTAMP[];
constexpr const char Metric::COMPONENT[];
constexpr const char Metric::COMPONENT_TYPE[];
constexpr const char Metric::VALUE[];
constexpr const char Metric::METRIC_DEFINITION[];
constexpr const char Metric::STATUS[];

constexpr const char MetricDefinition::METRIC_DEFINITION[];
constexpr const char MetricDefinition::NAME[];
constexpr const char MetricDefinition::DESCRIPTION[];
constexpr const char MetricDefinition::IMPLEMENTATION[];
constexpr const char MetricDefinition::CALCULABLE[];
constexpr const char MetricDefinition::UNITS[];
constexpr const char MetricDefinition::DATA_TYPE[];
constexpr const char MetricDefinition::IS_LINEAR[];
constexpr const char MetricDefinition::METRIC_TYPE[];
constexpr const char MetricDefinition::WILDCARDS[];
constexpr const char MetricDefinition::CALCULATION_PARAMETERS[];
constexpr const char MetricDefinition::PHYSICAL_CONTEXT[];
constexpr const char MetricDefinition::SENSOR_TYPE[];
constexpr const char MetricDefinition::SENSING_INTERVAL[];
constexpr const char MetricDefinition::DISCRETE_VALUES[];
constexpr const char MetricDefinition::PRECISION[];
constexpr const char MetricDefinition::ACCURACY[];
constexpr const char MetricDefinition::CALIBRATION[];
constexpr const char MetricDefinition::TIME_STAMP_ACCURACY[];
constexpr const char MetricDefinition::MIN_READING_RANGE[];
constexpr const char MetricDefinition::MAX_READING_RANGE[];
constexpr const char MetricDefinition::CALCULATION_ALGORITHM[];
constexpr const char MetricDefinition::CALCULATION_TIME_INTERVAL[];
constexpr const char MetricDefinition::CALCULATION_PRECISION[];
constexpr const char MetricDefinition::DISCRETE_METRIC_TYPE[];
constexpr const char MetricDefinition::SHOREUP_PERIOD[];

constexpr const char Wildcard::NAME[];
constexpr const char Wildcard::VALUES[];

constexpr const char CalculationParameters::SOURCE_METRIC[];
constexpr const char CalculationParameters::RESULT_METRIC[];

constexpr const char Zone::ZONE[];
constexpr const char Zone::STATUS[];
constexpr const char Zone::COLLECTIONS[];
constexpr const char Zone::OEM[];
constexpr const char Zone::FABRIC[];
constexpr const char Zone::ENDPOINTS[];
constexpr const char Zone::TASK[];

constexpr const char Endpoint::NAME[];
constexpr const char Endpoint::DESCRIPTION[];
constexpr const char Endpoint::ENDPOINT[];
constexpr const char Endpoint::FABRIC[];
constexpr const char Endpoint::PROTOCOL[];
constexpr const char Endpoint::IDENTIFIERS[];
constexpr const char Endpoint::CONNECTED_ENTITIES[];
constexpr const char Endpoint::IP_TRANSPORT_DETAILS[];
constexpr const char Endpoint::USERNAME[];
constexpr const char Endpoint::PASSWORD[];
constexpr const char Endpoint::COLLECTIONS[];
constexpr const char Endpoint::STATUS[];
constexpr const char Endpoint::OEM[];
constexpr const char Endpoint::NQN_FORMAT[];
constexpr const char Endpoint::TARGET[];
constexpr const char Endpoint::INITIATOR[];
constexpr const char Endpoint::PORTS[];
constexpr const char Endpoint::OEM_PROTOCOL[];

constexpr const char Capacity::ALLOCATED_BYTES[];
constexpr const char Capacity::CONSUMED_BYTES[];
constexpr const char Capacity::GUARANTEED_BYTES[];
constexpr const char Capacity::PROVISIONED_BYTES[];
constexpr const char Capacity::IS_THIN_PROVISIONED[];

constexpr const char CapacitySource::PROVIDING_DRIVES[];
constexpr const char CapacitySource::PROVIDING_VOLUMES[];
constexpr const char CapacitySource::PROVIDING_POOLS[];
}
}
}
