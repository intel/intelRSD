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
 * @file common.hpp
 * @brief Contains common constants used all over the model
 * */

#pragma once

namespace agent_framework {
namespace model {
namespace literals {

class ServiceUuid {
public:
    static constexpr const char UUID[] = "UUID";
};

/*!
 * @brief Class consisting of literals for Status model objects
 */
class Status {
public:
    static constexpr const char STATUS[] = "status";
    static constexpr const char STATE[] = "state";
    static constexpr const char HEALTH[] = "health";
};

/*!
 * @brief Class consisting of literals for FruInfo model objects
 */
class FruInfo {
public:
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char SERIAL[] = "serialNumber";
    static constexpr const char MANUFACTURER[] = "manufacturer";
    static constexpr const char MODEL[] = "modelNumber";
    static constexpr const char PART[] = "partNumber";
};

/*!
 * @brief Class consisting of literals for Oem model objects
 */
class Oem {
public:
    static constexpr const char OEM[] = "oem";
};

/*! @brief Used for Collections in each resource object */
class Collections {
public:
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char NAME[] = "name";
    static constexpr const char TYPE[] = "type";
};

/*! @brief Used for requests of getCollection methods */
class Collection {
public:
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char NAME[] = "name";
};

/*! @brief Used for responses of getCollectionMethod */
class SubcomponentEntry {
public:
    static constexpr const char SUBCOMPONENT[] = "subcomponent";
};

/*!
 * @brief Class consisting of literals for Component model objects
 */
class Component {
public:
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char ATTRIBUTES[] = "attributes";
    static constexpr const char ATTRIBUTE[] = "attribute";
    static constexpr const char NOTIFICATION[] = "notification";
    static constexpr const char PARENT[] = "parent";
    static constexpr const char TYPE[] = "type";
    static constexpr const char TIME_STAMP[] = "timeStamp";
    static constexpr const char MESSAGE[] = "message";
    static constexpr const char CODE[] = "code";
};

/*! @brief Used for getManagersCollections */
class ManagerEntry {
public:
    static constexpr const char MANAGER[] = "manager";
};

/*!
 * Class consisting of literals for TaskEntry model object
 * */
class TaskEntry {
public:
    static constexpr const char TASK[] = "task";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for SerialConsole model objects
 */
class SerialConsole {
public:
    static constexpr const char SERIAL_CONSOLE[] = "serialConsole";
    static constexpr const char SIGNAL_TYPE[] = "signalType";
    static constexpr const char BITRATE[] = "bitrate";
    static constexpr const char PARITY[] = "parity";
    static constexpr const char DATA_BITS[] = "dataBits";
    static constexpr const char STOP_BITS[] = "stopBits";
    static constexpr const char FLOW_CONTROL[] = "flowControl";
    static constexpr const char PIN_OUT[] = "pinOut";
    static constexpr const char ENABLED[] = "enabled";
    static constexpr const char MAX_SESSIONS[] = "maxSessions";
    static constexpr const char TYPES_SUPPORTED[] = "typesSupported";
};

/*!
 * @brief Class consisting of literals for graphical console model objects
 */
class GraphicalConsole {
public:
    static constexpr const char ENABLED[] = "enabled";
    static constexpr const char MAX_SESSIONS[] = "maxSessions";
    static constexpr const char TYPES_SUPPORTED[] = "typesSupported";
};

/*!
 * @brief Class consisting of literals for processor memory model objects
 */
class ProcessorMemory {
public:
    static constexpr char TYPE[] = "type";
    static constexpr char CAPACITY_MB[] = "capacityMB";
    static constexpr char SPEED_MHZ[] = "speedMhz";
};

/*!
 * @brief Class consisting of literals for command shell model objects
 */
class CommandShell {
public:
    static constexpr const char ENABLED[] = "enabled";
    static constexpr const char MAX_SESSIONS[] = "maxSessions";
    static constexpr const char TYPES_SUPPORTED[] = "typesSupported";
};

/*!
 * @brief Class consisting of literals for NetworkService model objects
 */
class NetworkService {
public:
    static constexpr const char NETWORK_SERVICES[] = "networkServices";
    static constexpr const char NAME[] = "name";
    static constexpr const char PORT[] = "port";
    static constexpr const char ENABLED[] = "enabled";
};

/*!
 * @brief Class consisting of literals for Manager model objects
 */
class Manager {
public:
    static constexpr const char MANAGER[] = "manager";
    static constexpr const char STATUS[] = "status";
    static constexpr const char TYPE[] = "type";
    static constexpr const char MODEL[] = "model";
    static constexpr const char LOCATION[] = "location";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char IPv4_ADDRESS[] = "ipv4Address";
    static constexpr const char NETWORK_SERVICES[] = "networkServices";
    static constexpr const char GRAPHICAL_CONSOLE[] = "graphicalConsole";
    static constexpr const char SERIAL_CONSOLE[] = "serialConsole";
    static constexpr const char COMMAND_SHELL[] = "commandShell";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char PARENT_ID[] = "parentId";
    static constexpr const char GUID[] = "guid";
    static constexpr const char DATE_TIME[] = "dateTime";
    static constexpr const char DATE_TIME_LOCAL_OFFSET[] = "dateTimeLocalOffset";
    static constexpr const char RESET[] = "reset";
    static constexpr const char FACTORY_DEFAULTS[] = "factoryDefaults";
    static constexpr const char PACKAGE_URL[] = "packageUrl";
    static constexpr const char ALLOWED_ACTIONS[] = "allowedActions";
};

/*!
 * @brief Class consisting of literals for NetworkInterface model objects
 */
class NetworkInterface {
public:
    static constexpr const char INTERFACE[] = "interface";
    static constexpr const char STATUS[] = "status";
    static constexpr const char FRAME_SIZE[] = "frameSize";
    static constexpr const char SPEED[] = "speedMbps";
    static constexpr const char AUTO_SENSE[] = "autoSense";
    static constexpr const char MAC_ADDRESS[] = "macAddress";
    static constexpr const char FACTORY_MAC_ADDRESS[] = "factoryMacAddress";
    static constexpr const char VLAN_ENABLE[] = "vlanEnable";
    static constexpr const char VLAN_COUNT[] = "vlanCount";
    static constexpr const char FULL_DUPLEX[] = "fullDuplex";
    static constexpr const char IPv4_ADDRESS[] = "ipv4Address";
    static constexpr const char IPv6_ADDRESS[] = "ipv6Address";
    static constexpr const char IPv4_ADDRESSES[] = "ipv4Addresses";
    static constexpr const char IPv6_ADDRESSES[] = "ipv6Addresses";
    static constexpr const char IPv6_DEFAULT_GATEWAY[] = "ipv6DefaultGateway";
    static constexpr const char MAX_IPv6_STATIC_ADDRESSES[] = "maxIPv6StaticAddresses";
    static constexpr const char DEFAULT_VLAN[] = "defaultVlan";
    static constexpr const char SUPPORTED_PROTOCOLS[] = "supportedProtocols";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*! @brief Class consisting of literals for Ipv4Address model objects */
class Ipv4Address {
public:
    static constexpr const char IPv4_ADDRESS[] = "ipv4Address";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char SUBNET_MASK[] = "subnetMask";
    static constexpr const char ADDRESS_ORIGIN[] = "addressOrigin";
    static constexpr const char GATEWAY[] = "gateway";
};

/*! @brief Class consisting of literals for Ipv6Address model objects */
class Ipv6Address {
public:
    static constexpr const char IPv6_ADDRESS[] = "ipv6Address";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char PREFIX_LENGTH[] = "prefixLength";
    static constexpr const char ADDRESS_ORIGIN[] = "addressOrigin";
    static constexpr const char ADDRESS_STATE[] = "addressState";
};

/*! @brief Intel literal */
class Intel {
public:
    static constexpr const char INTEL_CORP[] = "Intel Corporation";
};

/*! @brief Class consisting of literals for Drive model objects */
class Drive {
public:
    static constexpr const char DRIVE[] = "drive";
    static constexpr const char NAME[] = "name";
    static constexpr const char DESCRIPTION[] = "description";
    static constexpr const char STATUS[] = "status";
    static constexpr const char INTERFACE[] = "interface";
    static constexpr const char TYPE[] = "type";
    static constexpr const char RPM[] = "rpm";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char LATENCY_TRACKING_ENABLED[] = "latencyTrackingEnabled";
    static constexpr const char CAPACITY[] = "capacityGB";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char INDICATOR_LED[] = "indicatorLED";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char CAPABLE_SPEED_GBS[] = "capableSpeedGbs";
    static constexpr const char NEGOTIATED_SPEED_GBS[] = "negotiatedSpeedGbs";
    static constexpr const char LOCATION[] = "location";
    static constexpr const char STATUS_INDICATOR[] = "statusIndicator";
    static constexpr const char REVISION[] = "revision";
    static constexpr const char FAILURE_PREDICTED[] = "failurePredicted";
    static constexpr const char SKU[] = "sku";
    static constexpr const char IDENTIFIERS[] = "identifiers";
    static constexpr const char HOTSPARE_TYPE[] = "hotspareType";
    static constexpr const char ENCRYPTION_ABILITY[] = "encryptionAbility";
    static constexpr const char ENCRYPTION_STATUS[] = "encryptionStatus";
    static constexpr const char BLOCK_SIZE_BYTES[] = "blockSizeBytes";
    static constexpr const char PREDICTED_MEDIA_LIFE_LEFT[] = "predictedMediaLifeLeftPercent";
    static constexpr const char ERASED[] = "erased";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char SECURELY_ERASE[] = "securelyErase";
};

class Location {
public:
    static constexpr const char INFO[] = "info";
    static constexpr const char INFO_FORMAT[] = "infoFormat";
};

class Identifier {
public:
    static constexpr const char DURABLE_NAME[] = "durableName";
    static constexpr const char DURABLE_NAME_FORMAT[] = "durableNameFormat";
};

class Message {
public:
    static constexpr const char MESSAGE_ID[] = "messageId";
    static constexpr const char MESSAGE_CONTENT[] = "messageContent";
    static constexpr const char RELATED_PROPERTIES[] = "relatedProperties";
    static constexpr const char MESSAGE_ARGS[] = "messageArgs";
    static constexpr const char SEVERITY[] = "severity";
    static constexpr const char RESOLUTION[] = "resolution";
    static constexpr const char OEM[] = "oem";
};

class Task {
public:
    static constexpr const char TASK[] = "task";
    static constexpr const char STATE[] = "state";
    static constexpr const char START_TIME[] = "startTime";
    static constexpr const char END_TIME[] = "endTime";
    static constexpr const char STATUS[] = "status";
    static constexpr const char MESSAGES[] = "messages";
    static constexpr const char NAME[] = "name";
    static constexpr const char OEM[] = "oem";
};

class ConnectedEntity {
public:
    static constexpr const char ROLE[] = "role";
    static constexpr const char ENTITY[] = "entity";
    static constexpr const char IDENTIFIERS[] = "identifiers";
    static constexpr const char LUN[] = "lun";
};

class Transport {
public:
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char TYPE[] = "type";
    static constexpr const char DETAILS[] = "details";
};

class IpTransportDetail {
public:
    static constexpr const char IPV4_ADDRESS[] = "ipv4Address";
    static constexpr const char IPV6_ADDRESS[] = "ipv6Address";
    static constexpr const char PORT[] = "port";
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char INTERFACE[] = "interface";
};

class Metric {
public:
    static constexpr const char METRIC[] = "metric";
    static constexpr const char UUID[] = "uuid";
    static constexpr const char NAME[] = "name";
    static constexpr const char TIMESTAMP[] = "timestamp";
    static constexpr const char VALUE[] = "value";
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char COMPONENT_TYPE[] = "componentType";
    static constexpr const char METRIC_DEFINITION[] = "metricDefinition";
    static constexpr const char STATUS[] = "status";
};

class MetricDefinition {
public:
    static constexpr const char METRIC_DEFINITION[] = "metricDefinition";
    static constexpr const char NAME[] = "name";
    static constexpr const char DESCRIPTION[] = "description";
    static constexpr const char IMPLEMENTATION[] = "implementation";
    static constexpr const char CALCULABLE[] = "calculable";
    static constexpr const char UNITS[] = "units";
    static constexpr const char DATA_TYPE[] = "dataType";
    static constexpr const char IS_LINEAR[] = "isLinear";
    static constexpr const char METRIC_TYPE[] = "metricType";
    static constexpr const char WILDCARDS[] = "wildcards";
    static constexpr const char CALCULATION_PARAMETERS[] = "calculationParameters";
    static constexpr const char PHYSICAL_CONTEXT[] = "physicalContext";
    static constexpr const char SENSOR_TYPE[] = "sensorType";
    static constexpr const char SENSING_INTERVAL[] = "sensingInterval";
    static constexpr const char DISCRETE_VALUES[] = "discreteValues";
    static constexpr const char PRECISION[] = "precision";
    static constexpr const char ACCURACY[] = "accuracy";
    static constexpr const char CALIBRATION[] = "calibration";
    static constexpr const char TIME_STAMP_ACCURACY[] = "timeStampAccuracy";
    static constexpr const char MIN_READING_RANGE[] = "minReadingRange";
    static constexpr const char MAX_READING_RANGE[] = "maxReadingRange";
    static constexpr const char CALCULATION_ALGORITHM[] = "calculationAlgorithm";
    static constexpr const char CALCULATION_TIME_INTERVAL[] = "calculationTimeInterval";
    static constexpr const char CALCULATION_PRECISION[] = "calculationPrecision";
    static constexpr const char DISCRETE_METRIC_TYPE[] = "discreteMetricType";
    static constexpr const char SHOREUP_PERIOD[] = "shoreupPeriod";
};

/*!
 * @brief Class consisting of literals for Wildcards in MetricDefinition model objects
 */
class Wildcard {
public:
    static constexpr const char NAME[] = "name";
    static constexpr const char VALUES[] = "values";
};

/*!
 * @brief Class consisting of literals for CalculationParameters in MetricDefinition model objects
 */
class CalculationParameters {
public:
    static constexpr const char SOURCE_METRIC[] = "sourceMetric";
    static constexpr const char RESULT_METRIC[] = "resultMetric";
};

/*!
 * @brief Class consisting of literals for Zone
 */
class Zone {
public:
    static constexpr const char ZONE[] = "zone";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char FABRIC[] = "fabric";
    static constexpr const char ENDPOINTS[] = "endpoints";
    static constexpr const char TASK[] = "task";
};

/*!
 * @brief Class consisting of literals for Endpoint
 */
class Endpoint {
public:
    static constexpr const char NAME[] = "name";
    static constexpr const char DESCRIPTION[] = "description";
    static constexpr const char ENDPOINT[] = "endpoint";
    static constexpr const char FABRIC[] = "fabric";
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char CONNECTED_ENTITIES[] = "connectedEntities";
    static constexpr const char IDENTIFIERS[] = "identifiers";
    static constexpr const char STATUS[] = "status";
    static constexpr const char IP_TRANSPORT_DETAILS[] = "ipTransportDetails";
    static constexpr const char USERNAME[] = "username";
    static constexpr const char PASSWORD[] = "password";
    static constexpr const char NQN_FORMAT[] = "nqn.2014-08.org.nvmexpress:uuid:";
    static constexpr const char TARGET[] = "Target";
    static constexpr const char INITIATOR[] = "Initiator";
    static constexpr const char PORTS[] = "ports";
    static constexpr const char OEM_PROTOCOL[] = "oemProtocol";
};

/*!
 * @brief Capacity consisting of literals for Swordfish StoragePools and Volumes
 */
class Capacity {
public:
    static constexpr const char ALLOCATED_BYTES[] = "allocatedBytes";
    static constexpr const char CONSUMED_BYTES[] = "consumedBytes";
    static constexpr const char GUARANTEED_BYTES[] = "guaranteedBytes";
    static constexpr const char PROVISIONED_BYTES[] = "provisionedBytes";
    static constexpr const char IS_THIN_PROVISIONED[] = "isThinProvisioned";
};

/*!
 * @brief CapacitySource consisting of literals for Swordfish StoragePools and Volumes
 */
class CapacitySource {
public:
    static constexpr const char PROVIDING_DRIVES[] = "providingDrives";
    static constexpr const char PROVIDING_VOLUMES[] = "providingVolumes";
    static constexpr const char PROVIDING_POOLS[] = "providingPools";
};

}
}
}
