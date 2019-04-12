/*!
 * @brief Class to give access to IPMI for requested devices in RMM.
 *
 * All devices are identified by "slots" (location offset in the physical
 * rack).
 * The class is (to be) initialized on configuration data.
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
 * @file rmm/loader/ipmi_config.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"

#include <memory>
#include <vector>
#include <map>

namespace agent {
namespace rmm {
namespace loader {

class IpmiConfig final {
public:
    using LocationOffset = std::uint8_t;

    /*!
     * @brief Get instance of configuration for IPMI.
     * @return singleton instance
     */
    static IpmiConfig& get_instance();

    /*!
     * @brief Get list of configured slots
     * @return list of available location offset for zones.
     */
    std::vector<LocationOffset> get_controller_slots() const;

    /*!
     * @brief Get IPMI controller for given slot
     * @param slot of the zone to be used
     * @return IPMI controller for given slot
     */
    ipmi::IpmiController::Ptr get_controller(LocationOffset slot);

    /*!
     * @brief Get CM bridge info
     * @return bridge info to be used in send()
     */
    static ipmi::BridgeInfo get_cm_bridge();

    /*!
     * @brief Get MBP bridge info for given drawer chassis
     * @param chassis_slot location of the drawer chassis
     * @return bridge info to be used in send()
     */
    static ipmi::BridgeInfo get_mbp_bridge(std::uint8_t chassis_slot);

    /*!
     * @brief Get PSME-chassis bridge info for given drawer
     * @param drawer_slot location of the drawer
     * @return bridge info to be used in send()
     */
    static ipmi::BridgeInfo get_psme_chassis_bridge(std::uint8_t drawer_slot);

    /*!
     * @brief Add controller
     * @param slot location to select the controller
     * @param controller set controller
     */
    void add_controller(LocationOffset slot, ipmi::IpmiController::Ptr controller);

private:

    /*!
     * @brief Default constructor.
     */
    IpmiConfig();

    /*!
     * @brief Default destructor.
     */
    ~IpmiConfig();

    /*!
     * @brief Map with all handled IPMI controllers identified by slot
     */
    using ControllerMap = std::map<LocationOffset, ipmi::IpmiController::Ptr>;
    ControllerMap controllers{};
};

}
}
}
