/*!
 * @brief Compute agent grantley platform discoverer.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file grantley_discoverer.hpp
 */

#pragma once



#include "discovery/discoverers/generic_discoverer.hpp"
#include "smbios/smbios_parser.hpp"

#include <cstdint>
#include <vector>



namespace agent {
namespace compute {
namespace discovery {

class GrantleyDiscoverer : public GenericDiscoverer {
public:

    GrantleyDiscoverer(ipmi::IpmiController& management_controller,
                       ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory)
        : GenericDiscoverer(management_controller, mdr_accessor_factory) {}

    virtual ~GrantleyDiscoverer();

    std::uint32_t get_platform_id() const override;


    /*!
     * @brief Perform discovery of chassis object.
     * @param chassis Chassis object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_chassis(agent_framework::model::Chassis& chassis) override;


    /*!
     * @brief Perform discovery of processors objects.
     * @param processors Empty vector of processors objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_processors(std::vector<agent_framework::model::Processor>& processors,
                        const std::string& parent_uuid) override;


    /*!
     * @brief Perform discovery of memory objects.
     * @param memories Empty vector of memory objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_memory(std::vector<agent_framework::model::Memory>& memories, const std::string& parent_uuid) override;


    /*!
     * @brief Perform discovery of network interface objects.
     * @param network_interfaces Empty vector of network interface objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_network_interfaces(std::vector<agent_framework::model::NetworkInterface>& network_interfaces,
                                             const std::string& parent_uuid) override;


    /*!
     * @brief Perform network device function object.
     * @param network_device_function Network device function object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_network_device_function(agent_framework::model::NetworkDeviceFunction& network_device_function) override;


    /*!
     * @brief Perform discovery of system BIOS.
     * @param system System object to be updated with discovered data.
     * @return Updated system object.
     */
    virtual bool discover_bios(agent_framework::model::System& system) override;


    /*!
     * @brief Perform discovery of system cable ID.
     * @param system System object to be updated with discovered data.
     * @return Updated system object.
     */
    virtual bool discover_cable_id(agent_framework::model::System& system) override;
};

}
}
}
