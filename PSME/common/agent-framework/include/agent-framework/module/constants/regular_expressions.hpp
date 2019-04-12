/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file regular_expressions.hpp
 * @brief Contains constants used by regex validator
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {
namespace regex {

/*!
 * @brief Class consisting of literals for RemoteTarget regular expression validation
 */
class RemoteTarget {
public:
    static constexpr const char TARGET_IQN[] = "^iqn\\.[a-z0-9.:-]{1,200}";
    static constexpr const char INITIATOR_IQN[] = "^iqn\\.[a-z0-9.:-]{1,200}";
};

/*!
 * @brief Class consisting of literals for EthernetInterface regular expression validation
 */
class EthernetInterface {
public:
    static constexpr const char MAC_ADDRESS[] = "^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$";
};

/*!
 * @brief Class consisting of literals for more then one resource regular expression validation
 */
class Common {
public:
    static constexpr const char DEVICE_ID[] = "^0[xX](([a-fA-F]|[0-9]){2}){2}$";
    static constexpr const char VENDOR_ID[] = "^0[xX](([a-fA-F]|[0-9]){2}){2}$";
    static constexpr const char SUBSYSTEM_ID[] = "^0[xX](([a-fA-F]|[0-9]){2}){2}$";
    static constexpr const char SUBSYSTEM_VENDOR_ID[] = "^0[xX](([a-fA-F]|[0-9]){2}){2}$";
    static constexpr const char DATE_TIME_LOCAL_OFFSET[] = "([-+][0-1][0-9]:[0-5][0-9])";
    static constexpr const char NO_WHITESPACE_STRING[] = "^[\\S]+$";
    static constexpr const char EMPTY_OR_NO_WHITESPACE_STRING[] = "^$|^[\\S]+$";
};

/*!
 * @brief Class consisting of literals for PCIeFunction regular expression validation
 */
class PCIeFunction {
public:
    static constexpr const char CLASS_CODE[] = "^0[xX](([a-fA-F]|[0-9]){2}){3}$";
    static constexpr const char REVISION_ID[] = "^0[xX](([a-fA-F]|[0-9]){2}){1}$";
};

/*!
 * @brief Class consisting of literals for Event regular expression validation
 */
class Event {
public:
    static constexpr const char MESSAGE_ID[] = "^[A-Za-z0-9]+\\.\\d+\\.\\d+\\.[A-Za-z0-9.]+$";
};

/*!
 * @brief Class consisting of literals for ServiceRoot regular expression validation
 */
class ServiceRoot {
public:
    static constexpr const char REDFISH_VERSION[] = "^\\d+\\.\\d+\\.\\d+$";
};

/*!
 * @brief Class consisting of literals for Endpoint regular expression validation
 */
class Endpoint {
public:
    static constexpr const char PCI_CLASS_CODE[] = "^0[xX](([a-fA-F]|[0-9]){2}){3}$";
};

/*!
 * @brief Class consisting of literals for IPAddresses regular expression validation
 */
class IPAddresses {
public:
    static constexpr const char ADDRESS[] = "^(((([1]?\\d)?\\d|2[0-4]\\d|25[0-5])\\.){3})((([1]?\\d)?\\d|2[0-4]\\d|25[0-5]))$";
    static constexpr const char GATEWAY[] = "^(((([1]?\\d)?\\d|2[0-4]\\d|25[0-5])\\.){3})((([1]?\\d)?\\d|2[0-4]\\d|25[0-5]))$";
    static constexpr const char SUBNET_MASK[] = "^(((([1]?\\d)?\\d|2[0-4]\\d|25[0-5])\\.){3})((([1]?\\d)?\\d|2[0-4]\\d|25[0-5]))$";
};

/*!
 * @brief Class consisting of literals for IPv6Addresses regular expression validation
 */
class IPv6Addresses {
public:
    static constexpr const char ADDRESS[] = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))";
};

/*!
* @brief Class consisting of literals for Chassis regular expression validation
*/
class Chassis {
public:
    static constexpr const char LOCATION_ID[] = "^(?!\\s*$).+";
};

}
}
}
}
