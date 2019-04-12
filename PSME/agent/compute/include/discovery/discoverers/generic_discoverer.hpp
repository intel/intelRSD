/*!
 * @brief Compute agent generic discoverer interface.
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
 * @file generic_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "smbios/smbios_parser.hpp"
#include "acpi/acpi_parser.hpp"
#include "iscsi/iscsi_mdr_parser.hpp"
#include "ipmi/ipmi_controller.hpp"
#include "ipmi/utils/sdv/mdr_region_accessor.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <cstdint>
#include <vector>



namespace agent {
namespace compute {
namespace discovery {

class GenericDiscoverer {
public:

    using Ptr = std::shared_ptr<GenericDiscoverer>;

    GenericDiscoverer(ipmi::IpmiController& management_controller,
                      ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory)
        : m_management_controller(management_controller),
          m_mdr_accessor_factory{mdr_accessor_factory} {}


    virtual ~GenericDiscoverer();

    /*!
     * @return Platform id this discoverer supports.
     */
    virtual std::uint32_t get_platform_id() const = 0;

    /*!
     * @brief Retrieve MDR data from sled BMC.
     */
    virtual void read_mdr();

    /*!
     * @brief Set up SMBIOS parser with correct data.
     * @param smbios_data SMBIOS raw data blob.
     */
    void set_up_smbios_data(const std::vector<std::uint8_t>& smbios_data) {
        m_smbios_parser.reset(new smbios::parser::SmbiosParser(smbios_data.data(), smbios_data.size()));
    }

    /*!
     * @brief Set up ACPI parser with correct data.
     * @param acpi_data ACPI raw data blob.
     */
    void set_up_acpi_data(const std::vector<std::uint8_t>& acpi_data) {
        bool auxiliary_string_present = false;
        m_acpi_parser.reset(new acpi::parser::AcpiParser(acpi_data.data(), acpi_data.size(), auxiliary_string_present));
    }

    /*!
     * @brief Set up iSCSI parser with correct data.
     * @param iscsi_data iSCSI raw data blob.
     */
    void set_up_iscsi_data(const std::vector<std::uint8_t>& iscsi_data) {
        m_iscsi_parser.reset(new iscsi::parser::IscsiMdrParser(iscsi_data.data(), iscsi_data.size()));
    }


    /*!
     * @brief Perform discovery of a manager object.
     * @param manager Manager object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_manager(agent_framework::model::Manager& manager);


    /*!
     * @brief Perform discovery of chassis object.
     * @param chassis Chassis object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_chassis(agent_framework::model::Chassis& chassis);


    /*!
     * @brief Perform discovery of thermal zone object.
     * @param zone ThermalZone object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_thermal_zone(agent_framework::model::ThermalZone& zone);


    /*!
     * @brief Perform discovery of power zone object.
     * @param zone PowerZone object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_power_zone(agent_framework::model::PowerZone& zone);


    /*!
     * @brief Perform discovery of system object.
     * @param system System object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_system(agent_framework::model::System& system);


    /*!
     * @brief Perform discovery of PcieDevice objects.
     * @param devices Empty vector of PcieDevice objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @param chassis_uuid sled chassis UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_pcie_devices(std::vector<agent_framework::model::PcieDevice>& devices,
                          const Uuid& parent_uuid, const Uuid& chassis_uuid);

    /*!
     * @brief Perform discovery of PcieFunction objects.
     * @param functions Empty vector of PcieDevice objects, filled by the method with discovered data.
     * @param devices vector of PcieDevice objects, which are the parents for the functions
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_pcie_functions(std::vector<agent_framework::model::PcieFunction>& functions,
                           const std::vector<agent_framework::model::PcieDevice>& devices);

    /*!
     * @brief Perform discovery of processors objects.
     * @param processors Empty vector of processors objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_processors(std::vector<agent_framework::model::Processor>& processors, const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of memory objects.
     * @param memories Empty vector of memory objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_memory(std::vector<agent_framework::model::Memory>& memories, const Uuid& parent_uuid);

    /*!
     * @brief Perform discovery of memory domain objects.
     * @param memory_domains Empty vector of memory domain objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_memory_domains(std::vector<agent_framework::model::MemoryDomain>& memory_domains, const Uuid& parent_uuid);

    /*!
     * @brief Perform discovery of memory chunks objects.
     * @param memory_chunks Empty vector of memory chunks objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_memory_chunks(std::vector<agent_framework::model::MemoryChunks>& memory_chunks, const Uuid& parent_uuid);

    /*!
     * @brief Perform discovery of storage subsystem.
     * @param storage Storage subsystem object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_storage_subsystem(agent_framework::model::StorageSubsystem& storage);


    /*!
     * @brief Perform discovery of drives objects.
     * @param drives Empty vector of drive objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_drives(std::vector<agent_framework::model::Drive>& drives, const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of network interface objects.
     * @param network_interfaces Empty vector of network interface objects, filled by the method with discovered data.
     * @param parent_uuid Parent UUID.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_network_interfaces(std::vector<agent_framework::model::NetworkInterface>& network_interfaces,
                                             const Uuid& parent_uuid);


    /*!
     * @brief Perform discovery of network device object.
     * @param network_device Network device object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_network_device(agent_framework::model::NetworkDevice& network_device);


    /*!
     * @brief Perform network device function object.
     * @param network_device_function Network device function object to fill with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool
    discover_network_device_function(agent_framework::model::NetworkDeviceFunction& network_device_function);


    /*!
     * @brief Perform discovery of system BIOS.
     * @param system System object to be updated with discovered data.
     * @return Updated system object.
     */
    virtual bool discover_bios(agent_framework::model::System& system);


    /*!
     * @brief Perform discovery of system cable ID.
     * @param system System object to be updated with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_cable_id(agent_framework::model::System& system);


    /*!
     * @brief Perform discovery of trusted module.
     * @param trusted_modules  Vector of trusted module objects to be updated with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_trusted_modules(std::vector<agent_framework::model::TrustedModule>& trusted_modules,
                                          const Uuid& parent_uuid);


    /*!
     * @brief Discover system user mode.
     * @param system System object to be filled with discovered data.
     * @return True if discovery was successful, false otherwise.
     */
    virtual bool discover_user_mode(agent_framework::model::System& system);

    /*!
     * @brief Sets system to Rackscale mode. All systems managed by PSME should be in RackScale mode.
     * Along with user mode it prevents inband FW updates.
     * @param system System that is managed by the PSME
     * @return True if set was successful, false otherwise.
     */
    virtual bool set_rackscale_mode(agent_framework::model::System& system);


protected:
    /*!
     * @brief Get SMBIOS parser object.
     * @return SMBIOS parser object.
     */
    smbios::parser::SmbiosParser::Ptr get_smbios_parser() {
        return m_smbios_parser;
    }

    /*!
     * @brief Get iSCSI parser object.
     * @return iSCSI parser object.
     */
    iscsi::parser::IscsiMdrParser::Ptr get_iscsi_parser() {
        return m_iscsi_parser;
    }

    /*!
     * @brief Get ACPI parser object.
     * @return ACPI parser object.
     */
    acpi::parser::AcpiParser::Ptr get_acpi_parser() {
        return m_acpi_parser;
    }

    /*!
     * @brief Get ipmi controller object.
     * @return ipmi controller.
     */
    ipmi::IpmiController& get_management_controller() {
        return m_management_controller;
    }


    void log_ipmi_error(const ipmi::ResponseError& error) {
        log_error("legacy-discovery",
                  "An error occurred during execution of command 0x" << std::hex << static_cast<unsigned>(error.get_command()) << ", netfn 0x"
                                                                     << std::hex << static_cast<unsigned>(error.get_network_function())
                                                                     << ". Completion code: 0x"
                                                                     << std::hex
                                                                     << static_cast<unsigned>(error.get_completion_code())
                                                                     << ".");
    }


    void log_discovery_error(const std::exception& e, const std::string& resource_name) {
        log_error("legacy-discovery",
                  "An error occurred during discovery of " << resource_name << ": " << e.what() << ".");
    }


private:
    ipmi::IpmiController& m_management_controller;
    ipmi::sdv::MdrRegionAccessorFactory::Ptr m_mdr_accessor_factory;
    smbios::parser::SmbiosParser::Ptr m_smbios_parser{};
    acpi::parser::AcpiParser::Ptr m_acpi_parser{};
    iscsi::parser::IscsiMdrParser::Ptr m_iscsi_parser{};
};


/*!
 * @brief Class for creating concrete platform discoverers, depending on the type of platform
 */
class DiscovererFactory {
public:

    using Ptr = std::shared_ptr<DiscovererFactory>;

    /*!
     * Destructor
     */
    virtual ~DiscovererFactory();


    /*!
     * @brief Factory method
     * @param platform_id Platform identifier
     * @param ipmi_controller IpmiController
     * @param mdr_accessor_factory MdrRegionAccessorFactory
     * @return Pointer to Discoverer for given platform.
     */
    virtual GenericDiscoverer::Ptr create(ipmi::command::generic::BmcInterface,
                                          ipmi::IpmiController& ipmi_controller,
                                          ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory) const;
};

}
}
}
