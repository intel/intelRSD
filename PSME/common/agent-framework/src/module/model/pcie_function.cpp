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
 * */

#include "agent-framework/module/model/pcie_function.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component PcieFunction::component = enums::Component::PcieFunction;
const enums::CollectionName PcieFunction::collection_name =
    enums::CollectionName::PcieFunctions;

PcieFunction::PcieFunction(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

PcieFunction::~PcieFunction() {}

json::Json PcieFunction::to_json() const {
    json::Json result = json::Json();
    result[literals::PcieFunction::FUNCTION_ID] = get_function_id();
    result[literals::PcieFunction::DEVICE_CLASS] = get_device_class();
    result[literals::PcieFunction::FUNCTION_TYPE] = get_function_type();
    result[literals::PcieFunction::PCI_DEVICE_ID] = get_pci_device_id();
    result[literals::PcieFunction::PCI_VENDOR_ID] = get_pci_vendor_id();
    result[literals::PcieFunction::PCI_CLASS_CODE] = get_pci_class_code();
    result[literals::PcieFunction::PCI_REVISION_ID] = get_pci_revision_id();
    result[literals::PcieFunction::PCI_SUBSYSTEM_ID] = get_pci_subsystem_id();
    result[literals::PcieFunction::PCI_SUBSYSTEM_VENDOR_ID] = get_pci_subsystem_vendor_id();
    result[literals::PcieFunction::FUNCTIONAL_DEVICE] = get_functional_device();
    result[literals::PcieFunction::STATUS] = get_status().to_json();
    result[literals::PcieFunction::OEM] = get_oem().to_json();
    return result;
}

PcieFunction PcieFunction::from_json(const json::Json& json) {
    PcieFunction pcie_function;
    pcie_function.set_function_id(json[literals::PcieFunction::FUNCTION_ID]);
    pcie_function.set_device_class(json[literals::PcieFunction::DEVICE_CLASS]);
    pcie_function.set_function_type(json[literals::PcieFunction::FUNCTION_TYPE]);
    pcie_function.set_pci_device_id(json[literals::PcieFunction::PCI_DEVICE_ID]);
    pcie_function.set_pci_vendor_id(json[literals::PcieFunction::PCI_VENDOR_ID]);
    pcie_function.set_pci_class_code(json[literals::PcieFunction::PCI_CLASS_CODE]);
    pcie_function.set_pci_revision_id(json[literals::PcieFunction::PCI_REVISION_ID]);
    pcie_function.set_pci_subsystem_id(json[literals::PcieFunction::PCI_SUBSYSTEM_ID]);
    pcie_function.set_pci_subsystem_vendor_id(json[literals::PcieFunction::PCI_SUBSYSTEM_VENDOR_ID]);
    pcie_function.set_functional_device(json[literals::PcieFunction::FUNCTIONAL_DEVICE]);
    pcie_function.set_status(Status::from_json(json[literals::PcieFunction::STATUS]));
    pcie_function.set_oem(Oem::from_json(json[literals::PcieFunction::OEM]));
    return pcie_function;
}
