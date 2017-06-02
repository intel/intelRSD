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
 * @brief Class consisting of literals for HeartBeat
 */
class HeartBeat {
public:
    static constexpr const char TIME_STAMP[] = "timeStamp";
    static constexpr const char MIN_DELAY[] = "minDelay";
};

/*!
 * @brief Class consisting of literals for Oem model objects
 */
class Oem {
public:
    static constexpr const char OEM[] = "oem";
};

/* @brief Used for Collections in each resource object */
class Collections {
public:
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char NAME[] = "name";
    static constexpr const char TYPE[] = "type";
    static constexpr const char SLOT_MASK[] = "slotMask";
};

/* @brief Used for requests of getCollection methods */
class Collection {
public:
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char NAME[] = "name";
};

/* @brief Used for responses of getCollectionMethod */
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

/*!
 * @brief Class consisting of literals for Components model objects
 */
class Components {
public:
    static constexpr const char MANAGER[] = "manager";
};

/* @brief Used for getManagersCollections */
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

/* @brief Intel literal */
class Intel {
public:
    static constexpr const char INTEL_CORP[] = "Intel Corporation";
};

/*! @brief Class consisting of literals for Drive model objects */
class Drive {
public:
    static constexpr const char DRIVE[] = "drive";
    static constexpr const char STATUS[] = "status";
    static constexpr const char INTERFACE[] = "interface";
    static constexpr const char TYPE[] = "type";
    static constexpr const char RPM[] = "rpm";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char CAPACITY[] = "capacityGB";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char PHYSICAL_ID[] = "physicalId";
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
    static constexpr const char MESSAGE_ID[] = "MessageId";
    static constexpr const char MESSAGE[] = "Message";
    static constexpr const char RELATED_PROPERTIES[] = "RelatedProperties";
    static constexpr const char MESSAGE_ARGS[] = "MessageArgs";
    static constexpr const char SEVERITY[] = "Severity";
    static constexpr const char RESOLUTION[] = "Resolution";
    static constexpr const char OEM[] = "Oem";
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
    static constexpr const char ENTITY_TYPE[] = "entityType";
    static constexpr const char ENTITY_ROLE[] = "entityRole";
    static constexpr const char ENTITY[] = "entity";
};

}
}
}
