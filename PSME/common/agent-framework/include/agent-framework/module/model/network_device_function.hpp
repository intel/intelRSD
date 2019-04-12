/*!
 * @brief Declaration of NetworkDeviceFunction model class
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file network_device_function.hpp
 */

#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

class NetworkDeviceFunction : public Resource {
public:
    explicit NetworkDeviceFunction(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::NetworkDevice);


    NetworkDeviceFunction(const NetworkDeviceFunction&) = default;


    NetworkDeviceFunction& operator=(const NetworkDeviceFunction&) = default;


    NetworkDeviceFunction(NetworkDeviceFunction&&) = default;


    NetworkDeviceFunction& operator=(NetworkDeviceFunction&&) = default;


    ~NetworkDeviceFunction();


    /*!
     * @brief construct an object of class NetworkDeviceFunction from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed NetworkDeviceFunction object
     */
    static NetworkDeviceFunction from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return NetworkDeviceFunction::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return NetworkDeviceFunction::component;
    }

    /*!
     * @brief Get flag determining whether the network device function is enabled
     * @return flag determining whether the network device function is enabled
     * */
    bool get_device_enabled() const {
        return m_device_enabled;
    }

    /*!
     * @brief Set flag determining whether the network device function is enabled
     * @param[in] device_enabled the flag determining whether the network device function is enabled
     * */
    void set_device_enabled(const bool device_enabled) {
        m_device_enabled = device_enabled;
    }

    /*!
     * @brief Get the MAC address of the network device
     * @return the MAC address of the network device
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief Set the MAC address of the network device
     * @param[in] mac_address the the MAC address of the network device
     * */
    void set_mac_address(const OptionalField<std::string>& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * @brief Get the object containing iSCSI boot parameters
     * @return the object containing iSCSI boot parameters
     * */
    const attribute::IscsiBoot& get_iscsi_boot() const {
        return m_iscsi_boot;
    }

    /*!
     * @brief Set the object containing iSCSI boot parameters
     * @param[in] iscsi_boot the the object containing iSCSI boot parameters
     * */
    void set_iscsi_boot(const attribute::IscsiBoot& iscsi_boot) {
        m_iscsi_boot = iscsi_boot;
    }


private:
    bool m_device_enabled{};
    OptionalField<std::string> m_mac_address{};
    attribute::IscsiBoot m_iscsi_boot{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
