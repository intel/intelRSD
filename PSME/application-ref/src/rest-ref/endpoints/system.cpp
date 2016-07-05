/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/endpoints/system.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"

using namespace app::rest;
using namespace app::rest::constants;
using namespace psme::rest;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::utils;
using BootOverrideSupported =
        agent_framework::model::System::BootOverrideSupported;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ComputerSystem.1.0.0.ComputerSystem";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Computer System";
    r[Common::DESCRIPTION] = "Computer System description";
    r[System::SYSTEM_TYPE] = json::Value::Type::NIL;
    r[Common::ASSET_TAG] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[System::SKU] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[Common::UUID] = json::Value::Type::NIL;
    r[System::HOST_NAME] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[System::INDICATOR_LED] = json::Value::Type::NIL;
    r[System::POWER_STATE] = json::Value::Type::NIL;
    r[System::BIOS_VERSION] = json::Value::Type::NIL;

    json::Value boot;
    boot[System::BOOT_SOURCE_OVERRIDE_ENABLED] = json::Value::Type::NIL;
    boot[System::BOOT_SOURCE_OVERRIDE_TARGET] = json::Value::Type::NIL;
    boot[System::BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES] = json::Value::Type::ARRAY;
    r[System::BOOT] = std::move(boot);

    json::Value processors;
    processors[System::COUNT] = json::Value::Type::NIL;
    processors[Common::MODEL] = json::Value::Type::NIL;
    processors[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    processors[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    processors[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[System::PROCESSOR_SUMMARY] = std::move(processors);

    json::Value memory;
    memory[System::TOTAL_SYSTEM_MEMORY_GIB] = json::Value::Type::NIL;
    r[System::MEMORY_SUMMARY] = std::move(memory);

    r[System::PROCESSORS] = json::Value::Type::NIL;
    r[System::ETHERNET_INTERFACES] = json::Value::Type::NIL;

    r[System::DIMM_CONFIG] = json::Value::Type::NIL;
    r[System::MEMORY_CHUNKS] = json::Value::Type::NIL;

    r[Common::LINKS][System::CHASSIS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    json::Value allowable;
    allowable.push_back(System::ON);
    allowable.push_back(System::FORCE_OFF);
    allowable.push_back(System::GRACEFUL_SHUTDOWN);
    allowable.push_back(System::GRACEFUL_RESTART);
    allowable.push_back(System::FORCE_RESTART);

    json::Value actions;
    std::string hash_computer_system_reset("#");
    hash_computer_system_reset.append(constants::System::COMPUTER_SYSTEM_RESET);
    actions[hash_computer_system_reset][System::TARGET] = json::Value::Type::NIL;
    actions[hash_computer_system_reset][System::ALLOWABLE_RESET_TYPES] = std::move(allowable);
    r[Common::ACTIONS] = std::move(actions);

    json::Value rs;
    rs[System::STORAGE_ADAPTERS] = json::Value::Type::NIL;

    rs[System::PCI_DEVICES] = json::Value::Type::ARRAY;
    rs[System::PROCESSOR_SOCKETS] = json::Value::Type::NIL;
    rs[System::MEMORY_SOCKETS] = json::Value::Type::NIL;
    rs[System::DISCOVERY_STATE] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    return r;
}

void add_system_children(const agent_framework::model::System& s, json::Value& r){
    // PROCESSORS SUMMARY
    auto procs = agent_framework::module::get_manager<agent_framework::model::Processor>()
            .get_keys(s.get_uuid());

    r[constants::System::PROCESSOR_SUMMARY][Common::MODEL] = json::Value::Type::NIL;
    std::uint32_t processors_count = static_cast<std::uint32_t>(procs.size());
    if (processors_count > 0) {
        const auto& proc = agent_framework::module::get_manager<agent_framework::model::Processor>()
                .get_entry(procs.front());
        r[constants::System::PROCESSOR_SUMMARY][Common::MODEL] = proc.get_model_name();
    }

    r[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::STATE] = "Enabled";
    auto proc_rollup = psme::rest::endpoint::utils::
        rollup_children_health<agent_framework::model::System,
            agent_framework::model::Processor> (s).to_string();
    r[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::HEALTH] = proc_rollup;
    r[constants::System::PROCESSOR_SUMMARY][Common::STATUS][Common::HEALTH_ROLLUP] = proc_rollup;

    r[constants::System::PROCESSOR_SUMMARY][constants::System::COUNT] = processors_count;

    // MEMORY SUMMARY
    OptionalField<double> total_size_mb{};
    auto get_memory_size = [](const agent_framework::model::Dimm& m){return m.get_capacity_mb();};
    auto memory_sizes = psme::rest::endpoint::utils::query_entries
        <agent_framework::model::Dimm, OptionalField<double>>(s.get_uuid(), get_memory_size);

    for(auto& mem_size: memory_sizes) {
        total_size_mb = total_size_mb + mem_size;
    }

    r[constants::System::MEMORY_SUMMARY]
        [constants::System::TOTAL_SYSTEM_MEMORY_GIB] = ( total_size_mb.has_value() ?
            json::Value(uint32_t(endpoint::utils::mb_to_gib(total_size_mb))) :
            json::Value(json::Value::Type::NIL) );

    r[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::STATE] = "Enabled";
    auto memory_rollup = psme::rest::endpoint::utils::
        rollup_children_health<agent_framework::model::System,
            agent_framework::model::Dimm> (s).to_string();
    r[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::HEALTH] = memory_rollup;

    r[constants::System::MEMORY_SUMMARY][Common::STATUS][Common::HEALTH_ROLLUP] = memory_rollup;

    // PCI DEVICES
    for(auto& pci: s.get_pci_devices()){
        json::Value pci_device;
        pci_device[System::VENDOR_ID] = pci.get_vendor_id();
        pci_device[System::DEVICE_ID] = pci.get_device_id();
        r[Common::OEM][Common::RACKSCALE][System::PCI_DEVICES].push_back(std::move(pci_device));
    }
}

void make_children_links(const server::Request& req, json::Value& r) {
    using psme::rest::endpoint::PathBuilder;
    r[Common::OEM][Common::RACKSCALE][constants::System::STORAGE_ADAPTERS][Common::ODATA_ID] =
            PathBuilder(req).append(constants::System::STORAGE_ADAPTERS).build();
    r[constants::System::PROCESSORS][Common::ODATA_ID] =
            PathBuilder(req).append(constants::System::PROCESSORS).build();
    r[constants::System::ETHERNET_INTERFACES][Common::ODATA_ID] =
            PathBuilder(req).append(constants::System::ETHERNET_INTERFACES).build();
    r[constants::System::DIMM_CONFIG][Common::ODATA_ID] =
            PathBuilder(req).append(constants::System::DIMM_CONFIG).build();
    r[constants::System::MEMORY_CHUNKS][Common::ODATA_ID] =
            PathBuilder(req).append(constants::System::MEMORY_CHUNKS).build();
}

json::Value validate_patch_request(const server::Request& request,
                 const BootOverrideSupported& supported_values) {
    using json::Pair;
    using namespace psme;
    using namespace psme::rest::error;
    using namespace agent_framework::model::enums;

    // Convert vector to JSON array
    json::Value boot_override_target_supported{json::Value::Type::ARRAY};
    for(const auto& allowable : supported_values) {
        boot_override_target_supported.push_back(allowable.to_string());
    }

    json::Value boot_override_allowed{json::Value::Type::ARRAY};
    for(const auto& value : BootOverride::get_values()) {
        boot_override_allowed.push_back(value);
    }

    json::Value schema({
        Pair(constants::System::BOOT, json::Value({
            Pair(constants::System::BOOT_SOURCE_OVERRIDE_ENABLED,
                 JsonValidator::any_of(JsonValidator::STRING_TYPE,
                                       boot_override_allowed, true)),
            Pair(constants::System::BOOT_SOURCE_OVERRIDE_TARGET,
                 JsonValidator::any_of(JsonValidator::STRING_TYPE,
                                       boot_override_target_supported, true))
        }))
    });

    return JsonValidator::validate_request_body(request, schema);
}

void make_parent_links(const agent_framework::model::System& s, json::Value& r) {
    // find manager of this system
    try {
        json::Value managed_by;
        managed_by[Common::ODATA_ID] = "/redfish/v1/Managers/" +
            std::to_string(psme::rest::endpoint::utils::get_manager_by_uuid(
                    s.get_parent_uuid()).get_id());
        r[Common::LINKS][Common::MANAGED_BY].push_back(managed_by);
    } catch (agent_framework::exceptions::InvalidParameters&) {
    }

    // find chassis containing this system
    try {
        json::Value chassis;
        chassis[Common::ODATA_ID] = "/redfish/v1/Chassis/" +
            std::to_string(psme::rest::endpoint::utils::get_chassis_by_uuid(
                    s.get_chassis()).get_id());
        r[Common::LINKS][constants::System::CHASSIS].push_back(chassis);
    } catch (agent_framework::exceptions::InvalidParameters&) {
    }
}

}

endpoint::System::System(const std::string& path) : Endpoint(path) {}
endpoint::System::~System() {}



void endpoint::System::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();

    auto s = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();
    make_parent_links(s, r);

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::SYSTEM_ID];
    // do NOT remove, needed for podm integration purposes
    r[constants::System::SYSTEM_TYPE] = constants::System::PHYSICAL;
    r[Common::MANUFACTURER] = s.get_fru_info().get_manufacturer();
    r[Common::MODEL] = s.get_fru_info().get_model_number();
    r[Common::SERIAL] = s.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = s.get_fru_info().get_part_number();
    r[constants::Common::UUID] = s.get_guid();
    endpoint::utils::status_to_json(s,r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::utils::rollup_system_health(s.get_uuid()).to_string();
    r[constants::System::POWER_STATE] = s.get_power_state().to_string();
    r[constants::System::BIOS_VERSION] = s.get_bios_version();
    r[constants::System::SKU] = s.get_sku();
    r[constants::Common::ASSET_TAG] = s.get_asset_tag();
    r[constants::System::INDICATOR_LED] = s.get_indicator_led();

    r[constants::System::BOOT]
        [constants::System::BOOT_SOURCE_OVERRIDE_TARGET] =
            s.get_boot_override_target().to_string();

    r[constants::System::BOOT]
        [constants::System::BOOT_SOURCE_OVERRIDE_ENABLED] =
            s.get_boot_override().to_string();

    for(auto& allowable : s.get_boot_override_supported()) {
        r[constants::System::BOOT]
            [constants::System::BOOT_SOURCE_OVERRIDE_TARGET_ALLOWABLE_VALUES].
                push_back(allowable.to_string());
    }

    std::string hash_computer_system_reset("#");
    hash_computer_system_reset.append(constants::System::COMPUTER_SYSTEM_RESET);
    r[constants::Common::ACTIONS]
     [hash_computer_system_reset]
     [constants::System::TARGET] =
            PathBuilder(req).append(constants::Common::ACTIONS)
                            .append(constants::System::COMPUTER_SYSTEM_RESET)
                            .build();

    // do NOT remove, needed for podm integration purposes
    r[constants::Common::OEM][constants::Common::RACKSCALE]
            [constants::System::DISCOVERY_STATE] = constants::System::BASIC;
    add_system_children(s, r);
    make_children_links(req, r);

    set_response(res, r);
}

void endpoint::System::patch(const server::Request& request, server::Response& response) {

    std::string agent_id{};
    std::string system_uuid{};
    std::string parent_uuid{};
    BootOverrideSupported override_supported{};

    // Gets necessary data from model
    {
        auto system = model::Find<agent_framework::model::System>(
                                        request.params[PathParam::SYSTEM_ID]).get_one();

        agent_id =  system->get_agent_id();
        system_uuid = system->get_uuid();
        parent_uuid = system->get_parent_uuid();
        override_supported = system->get_boot_override_supported();
    }

    const auto& json = validate_patch_request(request, override_supported);

    const auto& boot_property = json[constants::System::BOOT];
    const auto& boot_override =
        boot_property[constants::System::BOOT_SOURCE_OVERRIDE_ENABLED].as_string();
    const auto& boot_override_target =
        boot_property[constants::System::BOOT_SOURCE_OVERRIDE_TARGET].as_string();

    agent_framework::model::attribute::Attributes attributes{};
    attributes.set_value(
        agent_framework::model::literals::System::BOOT_OVERRIDE, boot_override);
    attributes.set_value(
        agent_framework::model::literals::System::BOOT_OVERRIDE_TARGET,
        boot_override_target);

    agent_framework::model::requests::SetComponentAttributes
    set_component_attributes_request{system_uuid, attributes};

    // Call set component attribute method
    const auto& manager = psme::core::agent::AgentManager::get_instance();
    manager->call_method
        <agent_framework::model::responses::SetComponentAttributes>(
            agent_id, set_component_attributes_request);

    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System)->
        load(manager->get_agent(agent_id), parent_uuid,
             system_uuid, false);

    response.set_status(server::status_2XX::NO_CONTENT);
}
