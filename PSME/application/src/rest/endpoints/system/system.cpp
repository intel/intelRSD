/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/system.hpp"
#include "psme/rest/endpoints/system/system.hpp"
#include "psme/rest/endpoints/task_service/task_service_utils.hpp"
#include "psme/rest/endpoints/task_service/monitor_content_builder.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "utils/conversion.hpp"

/* Do not remove numeric header - needed by ARM compilation */
#include <numeric>



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::validators;
using namespace agent_framework::module;
using BootOverrideTarget = agent_framework::model::enums::BootOverrideTarget;
using BootOverrideSupported = agent_framework::model::System::BootOverrideSupported;

namespace {

    json::Json make_prototype() {
        json::Json response(json::Json::value_t::object);

        response[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ComputerSystem.ComputerSystem";
        response[Common::ODATA_ID] = json::Json::value_t::null;
        response[Common::ODATA_TYPE] = "#ComputerSystem.v1_3_0.ComputerSystem";
        response[Common::ID] = json::Json::value_t::null;
        response[Common::NAME] = "Computer System";
        response[Common::DESCRIPTION] = "Computer System description";
        response[System::SYSTEM_TYPE] = json::Json::value_t::null;
        response[Common::ASSET_TAG] = json::Json::value_t::null;
        response[Common::MANUFACTURER] = json::Json::value_t::null;
        response[Common::MODEL] = json::Json::value_t::null;
        response[Common::SKU] = json::Json::value_t::null;
        response[Common::SERIAL_NUMBER] = json::Json::value_t::null;
        response[Common::PART_NUMBER] = json::Json::value_t::null;
        response[Common::UUID] = json::Json::value_t::null;
        response[System::HOST_NAME] = json::Json::value_t::null;
        response[Common::STATUS][Common::STATE] = json::Json::value_t::null;
        response[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
        response[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
        response[System::INDICATOR_LED] = json::Json::value_t::null;
        response[System::POWER_STATE] = json::Json::value_t::null;
        response[System::BIOS_VERSION] = json::Json::value_t::null;
        response[System::HOSTED_SERVICES][System::STORAGE_SERVICES] = json::Json::value_t::array;
        response[System::HOSTING_ROLES] = json::Json::value_t::array;

        json::Json boot = json::Json();
        boot[Common::ODATA_TYPE] = "#ComputerSystem.v1_1_0.Boot";
        boot[System::BOOT_SOURCE_OVERRIDE_ENABLED] = json::Json::value_t::null;
        boot[System::BOOT_SOURCE_OVERRIDE_TARGET] = json::Json::value_t::null;
        boot[System::BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES] = json::Json::value_t::array;
        boot[System::BOOT_SOURCE_OVERRIDE_MODE] = json::Json::value_t::null;
        boot[System::BOOT_SOURCE_OVERRIDE_MODE_ALLOWABLE_VALUES] = json::Json::value_t::array;

        // Hardcode! Unlike BootOverrideSupported, there is no 'BootModeSupported' field on a system.
        for (const auto& boot_mode : agent_framework::model::enums::BootOverrideMode::get_values()) {
            boot[System::BOOT_SOURCE_OVERRIDE_MODE_ALLOWABLE_VALUES].push_back(boot_mode);
        }
        response[System::BOOT] = std::move(boot);

        json::Json processors = json::Json();
        processors[System::COUNT] = json::Json::value_t::null;
        processors[Common::MODEL] = json::Json::value_t::null;
        processors[Common::STATUS][Common::STATE] = json::Json::value_t::null;
        processors[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
        processors[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
        response[System::PROCESSOR_SUMMARY] = std::move(processors);

        json::Json memory = json::Json();
        memory[System::TOTAL_SYSTEM_MEMORY_GIB] = json::Json::value_t::null;
        memory[Common::STATUS][Common::STATE] = json::Json::value_t::null;
        memory[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
        memory[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
        response[System::MEMORY_SUMMARY] = std::move(memory);

        response[System::PROCESSORS] = json::Json::value_t::null;
        response[System::ETHERNET_INTERFACES] = json::Json::value_t::null;
        response[System::NETWORK_INTERFACES] = json::Json::value_t::null;
        response[System::STORAGE] = json::Json::value_t::null;
        response[System::MEMORY] = json::Json::value_t::null;
        response[Chassis::PCIE_DEVICES] = json::Json::value_t::array;
        response[PcieDevice::PCIE_FUNCTIONS] = json::Json::value_t::array;
        response[System::TRUSTED_MODULES] = json::Json::value_t::array;

        response[Common::LINKS][Common::ODATA_TYPE] = "#ComputerSystem.v1_2_0.Links";
        response[Common::LINKS][Common::CHASSIS] = json::Json::value_t::array;
        response[Common::LINKS][System::ENDPOINTS] = json::Json::value_t::array;
        response[Common::LINKS][Common::MANAGED_BY] = json::Json::value_t::array;
        response[Common::LINKS][Common::OEM] = json::Json::value_t::object;

        response[Common::ACTIONS] = json::Json::value_t::object;

        json::Json rs = json::Json();
        rs[Common::ODATA_TYPE] = "#Intel.Oem.ComputerSystem";
        rs[System::PCI_DEVICES] = json::Json::value_t::array;
        rs[System::PCIE_CONNECTION_ID] = json::Json::value_t::array;
        rs[System::PROCESSOR_SOCKETS] = json::Json::value_t::null;
        rs[System::MEMORY_SOCKETS] = json::Json::value_t::null;
        rs[System::USER_MODE_ENABLED] = json::Json::value_t::null;
        rs[System::TRUSTED_EXECUTION_TECHNOLOGY_ENABLED] = json::Json::value_t::null;
        rs[Common::METRICS] = json::Json::value_t::null;
        rs[Common::PERFORMANCE_CONFIGURATION] = json::Json::value_t::null;
        response[Common::OEM][Common::RACKSCALE] = std::move(rs);

        return response;
    }


    void add_reset_action(const server::Request& request, json::Json& response) {
        json::Json reset = json::Json();

        reset[Common::TARGET] = psme::rest::endpoint::PathBuilder(request)
                .append(constants::Common::ACTIONS)
                .append(constants::System::COMPUTER_SYSTEM_RESET)
                .build();

        json::Json allowable_reset_types = json::Json::value_t::array;
        for (const auto& reset_type : endpoint::System::get_allowed_reset_types()) {
            allowable_reset_types.push_back(reset_type.to_string());
        }
        reset[Common::ALLOWABLE_RESET_TYPES] = std::move(allowable_reset_types);

        response[Common::ACTIONS][System::HASH_COMPUTER_SYSTEM_RESET] = std::move(reset);
    }


    void add_dcpmem_erase_action(const server::Request& request, json::Json& response) {
        json::Json dcpmem_erase = json::Json();

        dcpmem_erase[Common::TARGET] = psme::rest::endpoint::PathBuilder(request)
                .append(constants::Common::ACTIONS)
                .append(constants::Common::OEM)
                .append(constants::System::INTEL_OEM_ERASE_OPTANE_DC_PERSISTENT_MEMORY)
                .build();

        json::Json allowable_values = json::Json::value_t::array;
        allowable_values.push_back(true);
        allowable_values.push_back(false);

        dcpmem_erase[System::ALLOWABLE_DCPMEM_RESET_CONFIGURATION_VALUES] = allowable_values;
        dcpmem_erase[System::ALLOWABLE_DCPMEM_ERASE_CONFIGURATION_KEY_VALUES] = allowable_values;

        response[Common::ACTIONS][Common::OEM][System::HASH_INTEL_OEM_ERASE_OPTANE_DC_PERSISTENT_MEMORY] = std::move(dcpmem_erase);
    }


    void add_processors_summary(const agent_framework::model::System& system, json::Json& response) {
        const agent_framework::model::enums::State ENABLED = agent_framework::model::enums::State::Enabled;

        // PROCESSORS SUMMARY
        auto procs = agent_framework::module::get_manager<agent_framework::model::Processor>()
                .get_keys(system.get_uuid());

        response[constants::System::PROCESSOR_SUMMARY][Common::MODEL] = json::Json::value_t::null;
        auto processors_count = static_cast<std::uint32_t>(procs.size());
        if (processors_count > 0) {
            const auto& proc = agent_framework::module::get_manager<agent_framework::model::Processor>()
                    .get_entry(procs.front());
            response[constants::System::PROCESSOR_SUMMARY][Common::MODEL] = proc.get_model_name();
        }

        response[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::STATE] =
                procs.empty() ? json::Json(nullptr) : json::Json(ENABLED.to_string());
        auto summarized_proc_health = psme::rest::endpoint::HealthRollup<agent_framework::model::System>()
                .get(system.get_uuid(), agent_framework::model::enums::Component::Processor);

        response[Common::OEM][Common::RACKSCALE][System::PROCESSOR_SOCKETS] = processors_count;
        response[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::HEALTH] = summarized_proc_health;
        response[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::HEALTH_ROLLUP] = summarized_proc_health;

        auto is_processor_present = [](const agent_framework::model::Processor& p) {
            return (p.get_status().get_state() == agent_framework::model::enums::State::Absent) ? 0 : 1;
        };
        auto available_processors =
                psme::rest::endpoint::utils::query_entries<agent_framework::model::Processor, std::uint32_t>(
                        system.get_uuid(), is_processor_present);
        response[constants::System::PROCESSOR_SUMMARY][constants::System::COUNT] =
                std::accumulate(available_processors.begin(), available_processors.end(), 0u);
    }


    void add_memory_summary(const agent_framework::model::System& system, json::Json& response) {
        const agent_framework::model::enums::State ENABLED = agent_framework::model::enums::State::Enabled;

        auto memory_modules = agent_framework::module::get_manager<agent_framework::model::Memory>()
                .get_keys(system.get_uuid());

        OptionalField<std::uint32_t> total_size_mb{};
        auto get_memory_size = [](const agent_framework::model::Memory& m) { return m.get_capacity_mib(); };
        auto memory_sizes = psme::rest::endpoint::utils::query_entries
                <agent_framework::model::Memory, OptionalField<std::uint32_t>>(system.get_uuid(), get_memory_size);

        for (const auto& mem_size: memory_sizes) {
            total_size_mb = total_size_mb + mem_size;
        }

        response[constants::System::MEMORY_SUMMARY][constants::System::TOTAL_SYSTEM_MEMORY_GIB] =
                (total_size_mb.has_value() ?
                 json::Json(endpoint::utils::round(
                         ::utils::convert_byte_units(total_size_mb, ::utils::BytePrefix::Mibi, ::utils::BytePrefix::Gibi), 2)) :
                 json::Json(json::Json::value_t::null));

        response[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::STATE] =
                memory_modules.empty() ? json::Json(nullptr) : json::Json(ENABLED.to_string());
        auto summarized_memory_health = psme::rest::endpoint::HealthRollup<agent_framework::model::System>()
                .get(system.get_uuid(), agent_framework::model::enums::Component::Memory);

        response[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::HEALTH] = summarized_memory_health;
        response[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::HEALTH_ROLLUP] = summarized_memory_health;

        response[Common::OEM][Common::RACKSCALE][System::MEMORY_SOCKETS] = memory_modules.size();
    }


    void add_system_relations(const agent_framework::model::System& system, json::Json& response) {
        // PCI Devices
        for (const auto& pci: system.get_pci_devices()) {
            json::Json pci_device = json::Json();
            pci_device[Common::VENDOR_ID] = pci.get_vendor_id();
            pci_device[Common::DEVICE_ID] = pci.get_device_id();
            response[Common::OEM][Common::RACKSCALE][System::PCI_DEVICES].push_back(std::move(pci_device));
        }

        // Cable IDs/PCIe Connections
        endpoint::utils::string_array_to_json(
                response[Common::OEM][Common::RACKSCALE][System::PCIE_CONNECTION_ID], system.get_cable_ids());

        // PCIe Devices
        // The manager for this system is also the manager for all the PCIe devices
        const auto& manager_uuid = system.get_parent_uuid();
        for (const auto& device_uuid :
                agent_framework::module::get_manager<agent_framework::model::PcieDevice>().get_keys(manager_uuid)) {
            json::Json device_link = json::Json();
            device_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                    agent_framework::model::enums::Component::PcieDevice, device_uuid);
            response[Chassis::PCIE_DEVICES].push_back(std::move(device_link));
        }

        // PCIe Functions
        // Per the architect's decision, we do not fill the PCIeFunctions array

        // Endpoints
        // Per the architect's decision, we do not fill the Endpoints array
    }


    void fill_storage_service_data(const agent_framework::model::System& system, json::Json& r) {
        using namespace agent_framework::model::enums;
        using agent_framework::model::System;
        using agent_framework::model::StorageService;

    r[constants::System::HOSTING_ROLES].push_back(HostingRole(HostingRole::StorageServer).to_string());

    try {
        json::Json hosted_services = json::Json();

        hosted_services[Common::ODATA_ID] = endpoint::PathBuilder(get_component_url(Component::System, system.get_uuid())).append(
            constants::System::STORAGE_SERVICES).build();
        r[constants::System::HOSTED_SERVICES][constants::System::STORAGE_SERVICES] = hosted_services;

    }
    catch (const agent_framework::exceptions::InvalidUuid&) {
        log_warning("rest", "System with UUID=" << system.get_uuid() << " don't exist");
    }

}


    void fill_tpm_data(const agent_framework::model::System& system, const server::Request& request, json::Json& r) {
        const auto& tpms =
                agent_framework::module::get_manager<agent_framework::model::TrustedModule>().get_entries(system.get_uuid());

        std::set<agent_framework::model::enums::InterfaceType> available_interface_types{};
        for (const auto& trusted_module : tpms) {
            json::Json tpm = json::Json();
            endpoint::status_to_json(trusted_module, tpm);

            tpm[Common::ODATA_TYPE] = "#ComputerSystem.v1_3_0.TrustedModules";
            tpm[Common::FIRMWARE_VERSION] = trusted_module.get_firmware_version();
            tpm[System::INTERFACE_TYPE] = trusted_module.get_interface_type();
            tpm[Common::OEM] = json::Json::value_t::object;
            tpm[System::FIRMWARE_VERSION_2] = json::Json::value_t::null;
            tpm[System::INTERFACE_TYPE_SELECTION] = System::OEM_METHOD;
            r[System::TRUSTED_MODULES].push_back(std::move(tpm));

            if (trusted_module.get_interface_type().has_value()) {
                available_interface_types.insert(trusted_module.get_interface_type().value());
            }
        }

        if (system.get_system_type() == agent_framework::model::enums::SystemType::Physical) {
            json::Json change_tpm_state = json::Json();
            change_tpm_state[constants::Common::TARGET] =
                    endpoint::PathBuilder(request)
                            .append(constants::Common::ACTIONS)
                            .append(constants::Common::OEM)
                            .append(constants::System::INTEL_OEM_CHANGE_TPM_STATE)
                            .build();

            change_tpm_state[constants::System::ALLOWABLE_INTERFACE_TYPE] = json::Json::value_t::array;
            for (auto interface_type : available_interface_types) {
                change_tpm_state[constants::System::ALLOWABLE_INTERFACE_TYPE].push_back(interface_type.to_string());
            }

            r[Common::ACTIONS][Common::OEM][constants::System::HASH_INTEL_OEM_CHANGE_TPM_STATE] =
                    std::move(change_tpm_state);
        }
    }


    void fill_performance_configuration_data(const agent_framework::model::System& system, json::Json& r) {

    if (!system.get_current_performance_configuration().has_value()) {
        return;
    }

        r[Common::OEM][Common::RACKSCALE][Common::PERFORMANCE_CONFIGURATION][Common::CURRENT_CONFIGURATION_ID] = system.get_current_performance_configuration();

        for (const auto& performance_configuration : system.get_performance_configurations()) {
            json::Json pc = json::Json();

            pc[PerformanceConfiguration::CONFIGURATION_ID] = performance_configuration.get_configuration_id();
            pc[PerformanceConfiguration::TYPE] = performance_configuration.get_type();
            pc[PerformanceConfiguration::TDP_PER_CPU] = performance_configuration.get_tdp();
            pc[PerformanceConfiguration::MAX_CPU_JUNCTION_TEMP] = performance_configuration.get_max_junction_temp_celsius();

        if (performance_configuration.get_type() ==
            agent_framework::model::enums::PerformanceConfigurationType::StaticSpeedSelect) {
            pc[Common::ODATA_TYPE] = "#Intel.Oem.SpeedSelectConfiguration";
            pc[PerformanceConfiguration::ACTIVE_CORES_PER_CPU] = performance_configuration.get_active_cores();
            pc[PerformanceConfiguration::BASE_CORE_FREQUENCY] = performance_configuration.get_base_core_frequency();
        }
        else if (performance_configuration.get_type() ==
                 agent_framework::model::enums::PerformanceConfigurationType::PrioritizedBaseFrequency) {
            pc[Common::ODATA_TYPE] = "#Intel.Oem.PrioritizedBaseFrequency";
            pc[PerformanceConfiguration::HIGH_PRIORITY_CORE_COUNT_PER_CPU] = performance_configuration.get_high_priority_core_count();
            pc[PerformanceConfiguration::HIGH_PRIORITY_BASE__CORE_FREQUENCY] = performance_configuration.get_high_priority_base_frequency();
            pc[PerformanceConfiguration::LOW_PRIORITY_CORE_COUNT_PER_CPU] = performance_configuration.get_low_priority_core_count();
            pc[PerformanceConfiguration::LOW_PRIORITY_BASE_CORE_FREQUENCY] = performance_configuration.get_low_priority_base_frequency();
        }

        r[Common::OEM][Common::RACKSCALE][Common::PERFORMANCE_CONFIGURATION][Common::CONFIGURATIONS].push_back(
            std::move(pc));
    }
}


    void make_children_links(const server::Request& req, json::Json& response) {
        using psme::rest::endpoint::PathBuilder;
        response[constants::System::PROCESSORS][Common::ODATA_ID] =
                PathBuilder(req).append(constants::System::PROCESSORS).build();
        response[constants::System::ETHERNET_INTERFACES][Common::ODATA_ID] =
                PathBuilder(req).append(constants::System::ETHERNET_INTERFACES).build();
        response[constants::System::STORAGE][Common::ODATA_ID] =
                PathBuilder(req).append(constants::System::STORAGE).build();
        response[constants::System::MEMORY][Common::ODATA_ID] =
                PathBuilder(req).append(constants::System::MEMORY).build();
        response[constants::System::NETWORK_INTERFACES][Common::ODATA_ID] =
                PathBuilder(req).append(constants::System::NETWORK_INTERFACES).build();
        response[Common::OEM][Common::RACKSCALE][constants::Common::METRICS][Common::ODATA_ID] =
                PathBuilder(req).append(constants::Common::METRICS).build();
    }


    static const std::map<std::string, std::string> gami_to_rest_attributes = {
            {agent_framework::model::literals::System::BOOT_OVERRIDE,
                    endpoint::PathBuilder(constants::System::BOOT)
                            .append(constants::System::BOOT_SOURCE_OVERRIDE_ENABLED).build()},
            {agent_framework::model::literals::System::BOOT_OVERRIDE_MODE,
                    endpoint::PathBuilder(constants::System::BOOT)
                            .append(constants::System::BOOT_SOURCE_OVERRIDE_ENABLED).build()},
            {agent_framework::model::literals::System::BOOT_OVERRIDE_TARGET,
                    endpoint::PathBuilder(constants::System::BOOT)
                            .append(constants::System::BOOT_SOURCE_OVERRIDE_TARGET).build()},
            {agent_framework::model::literals::System::USER_MODE_ENABLED,
                    endpoint::PathBuilder(constants::Common::OEM)
                            .append(constants::Common::RACKSCALE)
                            .append(constants::System::USER_MODE_ENABLED).build()},
            {agent_framework::model::literals::System::ASSET_TAG,
                    constants::Common::ASSET_TAG}
    };


    void make_parent_links(const agent_framework::model::System& system, json::Json& response) {
        // find manager of this system
        try {
            json::Json managed_by = json::Json();
            managed_by[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                    agent_framework::model::enums::Component::Manager, system.get_parent_uuid());
            response[Common::LINKS][Common::MANAGED_BY].push_back(managed_by);
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {}

        // find chassis containing this system
        try {
            if (system.get_chassis().has_value()) {
                json::Json chassis = json::Json();
                chassis[Common::ODATA_ID] =
                        psme::rest::endpoint::utils::get_component_url(agent_framework::model::enums::Component::Chassis,
                                                                       system.get_chassis());
                response[Common::LINKS][constants::Common::CHASSIS].push_back(chassis);
            }
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {}
    }


    void reload_network_device_function(psme::core::agent::JsonAgentSPtr gami_agent, const std::string& system_uuid) {
        const auto network_device_uuids =
                agent_framework::module::get_manager<agent_framework::model::NetworkDevice>().get_keys(system_uuid);
        if (network_device_uuids.empty() || network_device_uuids.size() > 1) {
            log_error("rest", "Invalid number of Network Devices for System: '" + system_uuid + "'!");
            return;
        }
        const auto network_function_uuids = agent_framework::module::get_manager<
                agent_framework::model::NetworkDeviceFunction>().get_keys(network_device_uuids.front());
        if (network_function_uuids.empty() || network_function_uuids.size() > 1) {
            log_error("rest",
                      "Invalid number of Network Device Functions for Device: '" + network_device_uuids.front() + "'!");
            return;
        }

        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::NetworkDeviceFunction)->
                load(gami_agent,
                     network_device_uuids.front(),
                     agent_framework::model::enums::Component::NetworkDevice,
                     network_function_uuids.front(),
                     false);
    }
}


endpoint::System::System(const std::string& path) : EndpointBase(path) {}


endpoint::System::~System() {}


const endpoint::System::AllowableResetTypes& endpoint::System::get_allowed_reset_types() {
    static endpoint::System::AllowableResetTypes allowed_reset_types{
            agent_framework::model::enums::ResetType::On,
            agent_framework::model::enums::ResetType::ForceOff,
            agent_framework::model::enums::ResetType::GracefulShutdown,
            agent_framework::model::enums::ResetType::GracefulRestart,
            agent_framework::model::enums::ResetType::ForceRestart
    };
    return allowed_reset_types;
}


void endpoint::System::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto system = psme::rest::model::find<agent_framework::model::System>(request.params).get();

    make_parent_links(system, r);

    r[constants::Common::ODATA_ID] = PathBuilder(request).build();
    r[constants::Common::ID] = request.params[PathParam::SYSTEM_ID];
    r[constants::System::SYSTEM_TYPE] = system.get_system_type();
    r[Common::MANUFACTURER] = system.get_fru_info().get_manufacturer();
    r[Common::MODEL] = system.get_fru_info().get_model_number();
    r[Common::SERIAL_NUMBER] = system.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = system.get_fru_info().get_part_number();
    r[constants::Common::UUID] = system.get_guid();

    endpoint::status_to_json(system, r);

    r[constants::System::POWER_STATE] = system.get_power_state();
    r[constants::System::BIOS_VERSION] = system.get_bios_version();
    r[constants::Common::SKU] = system.get_sku();
    r[constants::Common::ASSET_TAG] = system.get_asset_tag();
    r[constants::System::INDICATOR_LED] = system.get_indicator_led();
    r[constants::System::BOOT][constants::System::BOOT_SOURCE_OVERRIDE_TARGET] =
            system.get_boot_override_target();

    r[constants::System::BOOT][constants::System::BOOT_SOURCE_OVERRIDE_ENABLED] =
            system.get_boot_override();

    for (const auto& allowable : system.get_boot_override_supported()) {
        r[constants::System::BOOT][constants::System::BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES]
                .push_back(allowable.to_string());
    }

    if (system.get_boot_override_supported().empty()) {
        // if there are not allowed values for target - we show no allowed values for mode
        r[constants::System::BOOT][constants::System::BOOT_SOURCE_OVERRIDE_MODE_ALLOWABLE_VALUES] =
                json::Json::value_t::array;
    }

    r[constants::System::BOOT][constants::System::BOOT_SOURCE_OVERRIDE_MODE] =
            system.get_boot_override_mode();
    r[constants::Common::OEM][constants::Common::RACKSCALE]
    [constants::System::TRUSTED_EXECUTION_TECHNOLOGY_ENABLED] = system.is_txt_enabled();
    r[constants::Common::OEM][constants::Common::RACKSCALE]
    [constants::System::USER_MODE_ENABLED] = system.is_user_mode_enabled();

    add_processors_summary(system, r);
    add_memory_summary(system, r);
    add_system_relations(system, r);
    make_children_links(request, r);
    fill_tpm_data(system, request, r);
    fill_storage_service_data(system, r);
    fill_performance_configuration_data(system, r);

    if (system.get_system_type() == agent_framework::model::enums::SystemType::Physical) {
        add_reset_action(request, r);
    }

    add_dcpmem_erase_action(request, r);

    set_response(response, r);
}


void endpoint::System::patch(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PatchSystem";

    std::string agent_id{};
    std::string system_uuid{};
    std::string parent_uuid{};
    BootOverrideSupported override_supported{};
    bool task_received{false};

    // Gets necessary data from model
    auto system = model::find<agent_framework::model::System>(request.params).get();

    agent_id = system.get_agent_id();
    system_uuid = system.get_uuid();
    parent_uuid = system.get_parent_uuid();
    override_supported = system.get_boot_override_supported();

    const auto& json = JsonValidator::validate_request_body<schema::SystemPatchSchema>(request);
    agent_framework::model::attribute::Attributes attributes{};

    if (json.count(constants::Common::ASSET_TAG)) {
        attributes.set_value(agent_framework::model::literals::System::ASSET_TAG, json[constants::Common::ASSET_TAG]);
    }

    if (json.count(constants::System::BOOT)) {
        const auto& boot_property = json[constants::System::BOOT];
        if (boot_property.count(constants::System::BOOT_SOURCE_OVERRIDE_ENABLED)) {
            attributes.set_value(agent_framework::model::literals::System::BOOT_OVERRIDE,
                                 boot_property[constants::System::BOOT_SOURCE_OVERRIDE_ENABLED]
            );
        }
        if (boot_property.count(constants::System::BOOT_SOURCE_OVERRIDE_TARGET)) {
            const auto& boot_override_target = boot_property[constants::System::BOOT_SOURCE_OVERRIDE_TARGET].get<std::string>();
            const auto boot_override_target_enum = BootOverrideTarget::from_string(boot_override_target);

            if (!JsonValidator::validate_allowable_values(override_supported.get_array(), boot_override_target_enum)) {
                throw error::ServerException(
                        error::ErrorFactory::create_error_for_not_allowable_value(
                                "System does not support requested target.",
                                endpoint::PathBuilder(constants::System::BOOT)
                                        .append(constants::System::BOOT_SOURCE_OVERRIDE_TARGET)
                                        .build(),
                                boot_override_target,
                                endpoint::PathBuilder(constants::System::BOOT)
                                        .append(constants::System::BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES)
                                        .build()
                        )
                );
            }

            attributes.set_value(agent_framework::model::literals::System::BOOT_OVERRIDE_TARGET, boot_override_target);
        }
        if (boot_property.count(constants::System::BOOT_SOURCE_OVERRIDE_MODE)) {
            attributes.set_value(agent_framework::model::literals::System::BOOT_OVERRIDE_MODE,
                                 boot_property[constants::System::BOOT_SOURCE_OVERRIDE_MODE]
            );
        }
    }

    if (json.count(constants::Common::OEM)) {
        const auto& oem = json[constants::Common::OEM];
        if (oem.count(constants::Common::RACKSCALE)) {
            const auto& rackscale = oem[constants::Common::RACKSCALE];
            if (rackscale.count(constants::System::USER_MODE_ENABLED)) {
                attributes.set_value(agent_framework::model::literals::System::USER_MODE_ENABLED,
                                     rackscale[constants::System::USER_MODE_ENABLED]
                );
            }
            if (rackscale.count(constants::Common::PERFORMANCE_CONFIGURATION)) {
                const auto& performance_configuration = rackscale[constants::Common::PERFORMANCE_CONFIGURATION];
                if (performance_configuration.count(constants::Common::CURRENT_CONFIGURATION_ID)) {
                    attributes.set_value(agent_framework::model::literals::System::CURRENT_PERFORMANCE_CONFIGURATION,
                                         performance_configuration[constants::Common::CURRENT_CONFIGURATION_ID]);
                }
            }
        }
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes
                set_component_attributes_request{system_uuid, attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_system_attributes = [&, gami_agent] {
            // Call set component attribute method
            const auto& set_component_attributes_response = gami_agent->
                    execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                        result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            const auto task_uuid = set_component_attributes_response.get_task();
            task_received = !task_uuid.empty();

            if (task_received) {
                auto task_handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                        agent_framework::model::enums::Component::Task);
                task_handler->load(gami_agent,
                                   "",
                                   agent_framework::model::enums::Component::Task,
                                   task_uuid,
                                   false);

                auto response_renderer = [this, request](json::Json) -> server::Response {
                    Response finished_task_response{};
                    this->get(request, finished_task_response);
                    return finished_task_response;
                };

                auto completion_notifier = [system, agent_id](const std::string& set_task_uuid) {
                    auto lambda_agent_manager = psme::core::agent::AgentManager::get_instance();

                    {
                        auto task = agent_framework::module::get_manager<agent_framework::model::Task>()
                                .get_entry_reference(set_task_uuid);
                        if (task->get_state() == agent_framework::model::enums::TaskState::Completed) {
                            task->set_messages(task_service_utils::build_success_message());
                        }
                    }

                    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                            agent_framework::model::enums::Component::System)->
                            load(lambda_agent_manager->get_agent(agent_id),
                                 system.get_parent_uuid(), agent_framework::model::enums::Component::Manager,
                                 system.get_uuid(), false);
                };

                MonitorContentBuilder::get_instance()->add_builder(task_uuid, response_renderer);
                {
                    CommonComponents::get_instance()->get_task_manager()
                            .get_entry_reference(task_uuid)->
                            add_completion_notifier(std::bind(completion_notifier, task_uuid));
                }
                std::string task_monitor_url =
                        PathBuilder(utils::get_component_url(agent_framework::model::enums::Component::Task, task_uuid))
                                .append(Monitor::MONITOR).build();
                psme::rest::endpoint::utils::set_location_header(request, response, task_monitor_url);
                response.set_body(psme::rest::endpoint::task_service_utils::call_task_get(task_uuid).get_body());
            }
            else {
                psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                        agent_framework::model::enums::Component::System)->
                        load(gami_agent,
                             parent_uuid,
                             agent_framework::model::enums::Component::Manager,
                             system_uuid,
                             false);

                // setting system boot options can change Network Device Function. So, we reload it:
                reload_network_device_function(gami_agent, system_uuid);
            }
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, set_system_attributes);
    }
    task_received ? response.set_status(server::status_2XX::ACCEPTED) : get(request, response);
}
