/*!
 * @brief Compute agent Purley platform discoverer.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file purley_discoverer.hpp
 */

#pragma once



#include "discovery/discoverers/generic_discoverer.hpp"
#include "smbios/smbios_parser.hpp"

#include <cstdint>
#include <vector>



namespace agent {
namespace compute {
namespace discovery {

class PurleyDiscoverer : public GenericDiscoverer {
public:

    PurleyDiscoverer(ipmi::IpmiController& management_controller,
                     ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory)
        : GenericDiscoverer(management_controller, mdr_accessor_factory) {}


    virtual ~PurleyDiscoverer();


    std::uint32_t get_platform_id() const override;


    /*!
     * @brief Discover system user mode.
     * @param system System object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_user_mode(agent_framework::model::System& system) override;


    /*!
     * @brief Perform discovery of system cable ID.
     * @param system System object to be updated with discovered data.
     * @return Updated system object.
     */
    virtual bool discover_cable_id(agent_framework::model::System& system) override;


    /*!
     * @brief Perform discovery of chassis object.
     * @param chassis Chassis object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_chassis(agent_framework::model::Chassis& chassis) override;

    /*!
     * @brief Sets system to Rackscale mode. All systems managed by PSME should be in RackScale mode.
     * Along with user mode it prevents inband FW updates.
     * @param system System that is managed by the PSME
     * @return True if set was successful, false otherwise.
     */
    virtual bool set_rackscale_mode(agent_framework::model::System& system) override;

};
}
}
}
