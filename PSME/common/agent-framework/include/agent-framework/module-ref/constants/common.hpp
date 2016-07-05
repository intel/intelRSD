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
 *
 *
 * @file common.hpp
 * @brief Contains common constants used all over the model
 * */

#ifndef AGENT_FRAMEWORK_MODULE_CONSTANTS_COMMON_HPP
#define AGENT_FRAMEWORK_MODULE_CONSTANTS_COMMON_HPP

namespace agent_framework {
namespace model {
namespace literals {

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

// used for Collections in each resource object
class Collections {
public:
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char NAME[] = "name";
    static constexpr const char TYPE[] = "type";
    static constexpr const char SLOT_MASK[] = "slotMask";
};

// used for requests of getCollection methods
class Collection {
public:
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char NAME[] = "name";
};

// used for responses of getCollectionMethod
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
    static constexpr const char NOTIFICATION[] = "notification";
    static constexpr const char PARENT[] = "parent";
    static constexpr const char TYPE[] = "type";
    static constexpr const char TIME_STAMP[] = "timeStamp";
};

/*!
* @brief Class consisting of literals for Components model objects
*/
class Components {
public:
    static constexpr const char MANAGER[] = "manager";
};

// used for getManagerCollections
class ManagerEntry {
public:
    static constexpr const char MANAGER[] = "manager";
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
 * @brief Class consisting of literals for grafical console model objects
 */
class GraphicalConsole {
public:
    static constexpr const char ENABLED[] = "enabled";
    static constexpr const char MAX_SESSIONS[] = "maxSessions";
    static constexpr const char TYPES_SUPPORTED[] = "typesSupported";
};

/*!
 * @brief Class consisting of literals for grafical console model objects
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
    static constexpr const char GUID[] = "managerGuid";
};

/*!
* @brief Class consisting of literals for Ipv4Address model objects
*/
class Ipv4Address {
public:
    static constexpr const char IPv4_ADDRESS[] = "ipv4Address";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char SUBNET_MASK[] = "subnetMask";
    static constexpr const char ADDRESS_ORIGIN[] = "addressOrigin";
    static constexpr const char GATEWAY[] = "gateway";
};

/*!
* @brief Class consisting of literals for Ipv6Address model objects
*/
class Ipv6Address {
public:
    static constexpr const char IPv6_ADDRESS[] = "ipv6Address";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char PREFIX_LENGTH[] = "prefixLength";
    static constexpr const char ADDRESS_ORIGIN[] = "addressOrigin";
    static constexpr const char ADDRESS_STATE[] = "addressState";
};

// Intel literal
class Intel {
public:
    static constexpr const char INTEL_CORP[] = "Intel Corporation";
};

}
}
}

#endif // AGENT_FRAMEWORK_MODULE_CONSTANTS_COMMON_HPP
