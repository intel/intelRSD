/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file zone.hpp
 * @brief Definition of Fabric Zone class
 * */
#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/endpoint.hpp"



namespace agent_framework {
namespace model {

/*! @brief Zone class */
class Zone : public Resource {
public:
    explicit Zone(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Fabric);


    ~Zone();


    /*! Enable copy */
    Zone(const Zone&) = default;


    Zone& operator=(const Zone&) = default;


    /*! Enable move */
    Zone(Zone&&) = default;


    Zone& operator=(Zone&&) = default;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Zone::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return Zone::component;
    }


    /*!
     * @brief Returns JSON representation of the object.
     * @return Json representation.
     * */
    json::Json to_json() const;


    /*!
     * @brief Constructs object from JSON
     * @param[in] json the json::Json deserialized to object
     * @return the newly constructed object
     */
    static Zone from_json(const json::Json& json);


    /*!
     * @brief Get PCIe zone id
     * @return PCIe zone id
     * */
    OptionalField<uint8_t> get_zone_id() const {
        return m_zone_id;
    }


    /*!
     * @brief Set PCIe zone id
     * @param[in] zone_id PCIe zone id
     * */
    void set_zone_id(const OptionalField<uint8_t>& zone_id) {
        m_zone_id = zone_id;
    }


    /*!
     * @brief Gets switch uuid
     * @return switch uuid
     * */
    const std::string& get_switch_uuid() const {
        return m_switch_uuid;
    }


    /*!
     * @brief Sets switch uuid
     * @param[in] switch_uuid switch uuid
     * */
    void set_switch_uuid(const std::string& switch_uuid) {
        m_switch_uuid = switch_uuid;
    }


    /*!
     * @brief Checks if given zone has initiator endpoint
     * @param zone uuid of the zone to be inspected
     * @return true if zone has initiator endpoint, false otherwise
     */
    static bool is_initiator_in_zone(const Uuid& zone);


    /*!
     * @brief Checks if given zone has target endpoint
     * @param zone uuid of the zone to be inspected
     * @return true if zone has target endpoint, false otherwise
     */
    static bool is_target_in_zone(const Uuid& zone);


    /*!
     * @brief Returns UUIDs of devices connected to given zone
     * @param zone_uuid UUID of the zone
     * @return vector of UUIDs of devices connected to given zone
     */
    static std::vector<Uuid> get_connected_devices(const Uuid& zone_uuid);


    /*!
     * @brief Returns initiator endpoint for given zone uuid
     * @param zone_uuid UUID of the zone
     * @return initiator endpoint
     */
    static OptionalField<Endpoint> get_initiator_endpoint(const Uuid& zone_uuid);


private:

    OptionalField<uint8_t> m_zone_id{};

    std::string m_switch_uuid{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
