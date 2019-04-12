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

#include "psme/rest/endpoints/chassis/pcie_function.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#PCIeFunction.PCIeFunction";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#PCIeFunction.v1_0_0.PCIeFunction";
    r[Common::NAME] = "PCIe Function";

    r[constants::PcieFunction::CLASS_CODE] = json::Json::value_t::null;
    r[Common::DESCRIPTION] = "PCIe Function description";
    r[constants::PcieFunction::DEVICE_ID] = json::Json::value_t::null;
    r[constants::PcieFunction::FUNCTION_ID] = json::Json::value_t::null;
    // FunctionType and DeviceClass are only filled when it's available, because they're non-nullable
    r[Common::ID] = json::Json::value_t::null;
    r[Common::OEM] = json::Json::value_t::object;
    r[constants::PcieFunction::REVISION_ID] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[constants::PcieFunction::SUBSYSTEM_ID] = json::Json::value_t::null;
    r[constants::PcieFunction::SUBSYSTEM_VENDOR_ID] = json::Json::value_t::null;
    r[constants::PcieFunction::VENDOR_ID] = json::Json::value_t::null;

    r[Common::LINKS][constants::Chassis::DRIVES] = json::Json::value_t::array;
    r[Common::LINKS][constants::PcieFunction::PCIE_DEVICE] = json::Json::value_t::object;
    r[Common::LINKS][constants::StorageSubsystem::STORAGE_CONTROLLERS] = json::Json::value_t::array;
    r[Common::LINKS][constants::System::ETHERNET_INTERFACES] = json::Json::value_t::array;

    r[Common::LINKS][constants::Common::OEM][Common::RACKSCALE][constants::System::PROCESSORS] = json::Json::value_t::array;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.PCIeFunctionLinks";

    return r;
}


void fill_links(const agent_framework::model::PcieFunction& function,
                const std::string& device_uuid, json::Json& json) {

    json[Common::LINKS][constants::PcieFunction::PCIE_DEVICE][Common::ODATA_ID] =
        endpoint::utils::get_component_url(agent_framework::model::enums::Component::PcieDevice, device_uuid);

    if (!function.get_functional_device().has_value()) {
        return;
    }

    const auto functional_device_uuid = function.get_functional_device().value();

    if (get_manager<agent_framework::model::Drive>().entry_exists(functional_device_uuid)) {
        json::Json drive_link = json::Json();
        drive_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Drive, functional_device_uuid);
        json[Common::LINKS][constants::Chassis::DRIVES].push_back(std::move(drive_link));

        const auto storage_subsystems = agent_framework::module::CommonComponents::get_instance()
            ->get_storage_subsystem_drives_manager().get_parents(functional_device_uuid);

        auto& controller_manager = agent_framework::module::ComputeComponents::get_instance()->get_storage_controller_manager();

        for (const auto& storage_uuid : storage_subsystems) {
            const auto controllers = controller_manager.get_keys(storage_uuid);
            for (const auto& controller_uuid: controllers) {
                json::Json controller_link = json::Json();
                controller_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                    agent_framework::model::enums::Component::StorageController, controller_uuid);
                json[Common::LINKS][constants::StorageSubsystem::STORAGE_CONTROLLERS].push_back(
                    std::move(controller_link));
            }
        }
    }
    else if (get_manager<agent_framework::model::Processor>().entry_exists(functional_device_uuid)) {
        json::Json processor_link = json::Json();
        processor_link[Common::ODATA_ID] = endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Processor, functional_device_uuid);
        json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::System::PROCESSORS].push_back(std::move(processor_link));
    }
    else {
        log_error("rest", "Function " + function.get_uuid() + " has functional device "
                          + function.get_functional_device().value() +
                          " which does not exist as a resource");
    }
}


agent_framework::model::PcieDevice find_pci_device(const server::Request& req) {

    // the devices are under the same manager as the chassis from the URL
    auto chassis_manager_uuid = psme::rest::model::find<agent_framework::model::Chassis>(
        req.params).get_one()->get_parent_uuid();

    auto chassis_manager_id = agent_framework::module::get_manager<agent_framework::model::Manager>().get_entry(
        chassis_manager_uuid).get_id();

    auto params_copy = req.params;
    params_copy[constants::PathParam::MANAGER_ID] = std::to_string(chassis_manager_id);

    return psme::rest::model::find<agent_framework::model::Manager, agent_framework::model::PcieDevice>(
        params_copy).get();
}

}


endpoint::PcieFunction::PcieFunction(const std::string& path) : EndpointBase(path) {}


endpoint::PcieFunction::~PcieFunction() {}


void endpoint::PcieFunction::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto device = find_pci_device(req);

    const auto function = psme::rest::model::find<agent_framework::model::PcieDevice, agent_framework::model::PcieFunction>(
        req.params).get();

    json[Common::ID] = req.params[PathParam::FUNCTION_ID];

    fill_links(function, device.get_uuid(), json);

    json[constants::PcieFunction::CLASS_CODE] = function.get_pci_class_code();
    json[constants::PcieFunction::DEVICE_ID] = function.get_pci_device_id();
    try {
        if (function.get_function_id().has_value()) {
            json[constants::PcieFunction::FUNCTION_ID] = std::stoi(function.get_function_id());
        }
    }
    catch (const std::exception& ex) {
        log_warning("rest", "Invalid function id type:" << ex.what());
    }
    json[constants::PcieFunction::REVISION_ID] = function.get_pci_revision_id();
    if (function.get_device_class().has_value()) {
        json[constants::PcieFunction::DEVICE_CLASS] = function.get_device_class();
    }
    if (function.get_function_type().has_value()) {
        json[constants::PcieFunction::FUNCTION_TYPE] = function.get_function_type();
    }

    endpoint::status_to_json(function, json);
    json[Common::STATUS][Common::HEALTH_ROLLUP] = function.get_status().get_health();

    json[constants::PcieFunction::SUBSYSTEM_ID] = function.get_pci_subsystem_id();
    json[constants::PcieFunction::SUBSYSTEM_VENDOR_ID] = function.get_pci_subsystem_vendor_id();
    json[constants::PcieFunction::VENDOR_ID] = function.get_pci_vendor_id();

    set_response(res, json);
}
