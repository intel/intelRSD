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

#include "agent-framework/module/model/attributes/pci_device.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

PciDevice::PciDevice() { }

PciDevice::~PciDevice() { }

json::Json attribute::PciDevice::to_json() const {
    json::Json json = json::Json();
    json[literals::PciDevice::DEVICE_ID] = get_device_id();
    json[literals::PciDevice::VENDOR_ID] = get_vendor_id();
    return json;
}

PciDevice PciDevice::from_json(const json::Json& json) {
    attribute::PciDevice pci_device;
    pci_device.set_device_id(json[literals::PciDevice::DEVICE_ID]);
    pci_device.set_vendor_id(json[literals::PciDevice::VENDOR_ID]);
    return pci_device;
}
