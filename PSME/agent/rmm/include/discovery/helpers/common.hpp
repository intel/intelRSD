/*!
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
 * @file rmm/discovery/helpers/common.hpp
 */

#pragma once

#include "agent-framework/module/enum/chassis.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "discovery/helpers/types.hpp"
#include "tree_stability/rmm_tree_stabilizer.hpp"
#include "ipmi/command/generic/get_lan_config_params.hpp"

#include "ipmi/ipmi_controller.hpp"
#include "net_reader/net_reader_interface.hpp"
#include "ptas_reader/ptas_reader_interface.hpp"
#include "event_collector/event_collector_interface.hpp"
#include "certificate_management/certificate_manager_interface.hpp"
#include "loader/ipmi_config.hpp"
#include "telemetry/rmm_telemetry_service_interface.hpp"

#include <type_traits>

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

namespace attribute = agent_framework::model::attribute;
namespace enums = agent_framework::model::enums;

/*! Enum representing action to be taken during discovery */
enum class DiscoveryAction : uint8_t {None, Add, Remove, Update};

/*! This class contains all access interfaces that are used for discovery */
struct DiscoveryContext final {
    std::shared_ptr<ipmi::IpmiController> ipmi{};
    std::shared_ptr<net_reader::NetReaderInterface> net_reader{};
    std::shared_ptr<ptas::PtasReaderInterface> ptas{};
    std::shared_ptr<event_collector::EventCollectorInterface> event_collector{};
    std::shared_ptr<agent::rmm::CertificateManagerInterface> certificate_manager{};
    std::shared_ptr<agent::rmm::RmmTelemetryServiceInterface> telemetry_service{};
};

/*!
 * In some cases, during the discovery/update of a resource or its metrics, some additional parameters have to be passed,
 * these structs define what is passed to discovery/update procedures of specific resources. By default - these structs
 * are empty.
 * E.g.:  - psus/fans/drawers require information about their slot (needed for identification)
 *        - drawers additionally require information about parent chassis zone, power zone, thermal zone etc.
 * */
template<RmmType TYPE>
struct DiscoveryParams final {
};

template<>
struct DiscoveryParams<RmmType::Psu> final {
    uint8_t slot_id{};
};

template<>
struct DiscoveryParams<RmmType::Fan> final {
    uint8_t slot_id{};
    uint16_t tachs{};
};

template<>
struct DiscoveryParams<RmmType::Drawer> final {
    uint8_t slot_id{};
    uint8_t location_offset{};
    std::string pzone_uuid{};
    std::string tzone_uuid{};
    std::string zone_uuid{};
    bool is_alert_present{};
    std::string parent_id{};
};

template<>
struct DiscoveryParams<RmmType::Zone> final {
    std::string rack_uuid{};
    uint8_t location_offset{};
    std::string manufacturer_name{};
    std::string product_name{};
    std::string parent_id{};
};

template<>
struct DiscoveryParams<RmmType::PowerZone> final {
    std::vector<agent_framework::model::Psu> psus{};
    std::vector<std::uint8_t> drawer_slots{};
};

template<>
struct DiscoveryParams<RmmType::ThermalZone> final {
    std::vector<std::uint8_t> fan_slots{};
    std::vector<std::uint8_t> drawer_slots{};
};


template<>
struct DiscoveryParams<RmmType::ZoneManager> final {
    std::string firmware_version{};
};

template<>
struct DiscoveryParams<RmmType::RackInterface> final {
    net_reader::RawNetworkInterface raw_data{};
};

template<>
struct DiscoveryParams<RmmType::RackVlan> final {
    uint32_t vlan_id{};
};



/*!
 * @brief Generates DiscoveryAction for any combination of was_present/is_present flags
 * @param[in] was_present Flag stating if resource was present before
 * @param[in] is_present Flag stating if resource is present right now
 * @return Type of action to be taken (Add/Remove/Update/None)
 * */
DiscoveryAction get_action_type(bool was_present, bool is_present);

/*!
 * @brief Converts IPMI address source enum into model address origin
 * @param[in] source Ipmi address source to be converted
 * @return Address source converted to model address origin enum, may be kept null if value was not recognized
 * */
OptionalField<enums::Ipv4AddressOrigin> convert_ipmi_address_source_to_model_address_origin(
        ipmi::command::generic::LanConfigIpAddressSource source);

/*!
 * @brief Converts string to optional string (not set value for empty strings)
 * @param str String to be converted
 * @return Optional string value
 * */
OptionalField<std::string> to_optional_string(const std::string& str);

using StabilizeFnPtr = const std::string (agent::rmm::RmmTreeStabilizer::*)(const std::string&) const;

/*!
 * @brief Tool function used make non-template stabilize functions comaptible with templates - calls specific `stabilize`
 *        function and handles exceptions
 * @param component Type of component to be stabilized
 * @param uuid Uuid of the resource to be stabilized
 * @param fn RmmTreeStabilizer method to be called
 * @return Stabilized uuid of the resource
 * */
std::string generic_stabilizer(const enums::Component& component, const std::string& uuid, StabilizeFnPtr fn);

/*!
 * @brief Template method used to shorten psu ipmi commands calls
 * */
template <typename REQ, typename RSP>
RSP call_psu_command(const DiscoveryContext& dc, uint8_t slot_id) {
    REQ req{};
    RSP rsp{};
    req.set_psu_id(slot_id);
    dc.ipmi->send(req, rsp);
    return rsp;
}

}
}
}
}
