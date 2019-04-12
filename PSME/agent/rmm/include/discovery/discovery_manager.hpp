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
 * @file rmm/discovery/discovery_manager.hpp
 */

#pragma once

#include "discovery/helpers/common.hpp"
#include "net_reader/net_reader.hpp"

#include <string>
#include <memory>

namespace agent {
namespace rmm {
namespace discovery {

/*!
 * Discovery manager handles resource discovery, updates etc
 */
class DiscoveryManager final {

public:

    /*!
     * @brief Default constructor.
     */
    DiscoveryManager() {}

    /*!
     * Enable copy
     */
    DiscoveryManager(const DiscoveryManager&) = default;

    /*!
     * @brief Default destructor.
     */
    ~DiscoveryManager();

    /*!
     * @brief Handles zone discovery cycle
     * @param dc Discovery Context used for discovery
     * @param[in] location_offset Offset of the zone
     */
    void zone_discovery(const helpers::DiscoveryContext& dc, uint8_t location_offset);

    /*!
     * @brief Handles rack discovery cycle
     * @param dc Discovery Context used for discovery
     * */
    void rack_discovery(const helpers::DiscoveryContext& dc);

private:

    using ZoneDiscoveryParams = helpers::DiscoveryParams<helpers::RmmType::Zone>;
    using DrawerDiscoveryParams = helpers::DiscoveryParams<helpers::RmmType::Drawer>;

    /* handles zone discovery (similar to handle_resource template) */
    void handle_zone(const helpers::DiscoveryContext& dc, ZoneDiscoveryParams& dp, bool is_present);

    /* handles zone add operation */
    void add_zone(const helpers::DiscoveryContext& dc, ZoneDiscoveryParams& dp);

    /* handles zone update operation */
    void update_zone(const helpers::DiscoveryContext& dc, ZoneDiscoveryParams& dp, const std::string& uuid);

    /* handles zone remove operation */
    void remove_zone(const helpers::DiscoveryContext& dc, ZoneDiscoveryParams& dp, const std::string& uuid);

    /* returns true if zone is the model */
    bool is_zone_present(const helpers::DiscoveryContext& dc, ZoneDiscoveryParams&);

    /* handles psu collection discovery */
    void handle_psus(const helpers::DiscoveryContext& dc, const std::string& pzone);

    /* handles fan collection discovery */
    void handle_fans(const helpers::DiscoveryContext& dc, const std::string& tzone);

    /* handles network interface and vlan discovery */
    void handle_interface_with_vlans(const helpers::DiscoveryContext& dc,
                                     const net_reader::RawNetworkInterface& interface,
                                     const std::string& manager_uuid);

    /*
     * @brief handles drawer collection discovery
     *
     * @param dc Discovery Context used for discovery
     * @param dp additional parameters used to update the drawers
     * @param[in,out] drawer_slots vector of non-empty drawer slots - filled by handle_drawers for future use
     */
    void handle_drawers(const helpers::DiscoveryContext& dc, DrawerDiscoveryParams& ddp, std::vector<std::uint8_t>& drawer_slots);

    /* handles ptas reading */
    void handle_ptas(const helpers::DiscoveryContext& dc, const std::string& zone_uuid, const std::string& pzone_uuid,
        const std::string& tzone_uuid);

    /* handles rack add operation */
    void add_rack(const helpers::DiscoveryContext& dc);

    /* handles rack update operation */
    void update_rack(const helpers::DiscoveryContext& dc, const std::string& rack_uuid);

    /* sends location id to the drawer chassis */
    void send_location_id(const helpers::DiscoveryContext& dc, const DrawerDiscoveryParams& dp,
        const OptionalField<std::string>& uuid) const;

    std::string m_rack_uuid{};
    std::string m_rack_manager_uuid{};

};

}
}
}
