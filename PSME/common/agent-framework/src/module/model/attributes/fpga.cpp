/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file fpga.cpp
 * @brief Fpga class implementation
 * */

#include "agent-framework/module/model/attributes/fpga.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/compute.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

json::Json Fpga::to_json() const {
    json::Json json = json::Json();

    json[literals::Fpga::TYPE] = m_type;
    json[literals::Fpga::MODEL] = m_model;
    json[literals::Fpga::FIRMWARE_ID] = m_firmware_id;
    json[literals::Fpga::FIRMWARE_MANUFACTURER] = m_firmware_manufacturer;
    json[literals::Fpga::FIRMWARE_VERSION] = m_firmware_version;
    json[literals::Fpga::HOST_INTERFACE] = m_host_interface;
    json[literals::Fpga::PCIE_VIRTUAL_FUNCTIONS] = m_pcie_virtual_functions;
    json[literals::Fpga::PROGRAMMABLE_FROM_HOST] = m_programmable_from_host;
    json[literals::Fpga::RECONFIGURATION_SLOTS] = m_reconfiguration_slots.to_json();
    json[literals::Fpga::ERASED] = m_erased;

    return json;
}

Fpga Fpga::from_json(const json::Json& json) {
    Fpga fpga{};

    fpga.set_type(json.value(literals::Fpga::TYPE, OptionalField<enums::FpgaType>()));
    fpga.set_model(json.value(literals::Fpga::MODEL, OptionalField<std::string>()));
    fpga.set_firmware_id(json.value(literals::Fpga::FIRMWARE_ID, OptionalField<std::string>()));
    fpga.set_firmware_manufacturer(json.value(literals::Fpga::FIRMWARE_MANUFACTURER, OptionalField<std::string>()));
    fpga.set_firmware_version(json.value(literals::Fpga::FIRMWARE_VERSION, OptionalField<std::string>()));
    fpga.set_host_interface(json.value(literals::Fpga::HOST_INTERFACE, OptionalField<enums::FpgaInterface>()));
    fpga.set_pcie_virtual_functions(json.value(literals::Fpga::PCIE_VIRTUAL_FUNCTIONS, OptionalField<std::uint32_t>()));
    fpga.set_programmable_from_host(json.value(literals::Fpga::PROGRAMMABLE_FROM_HOST, OptionalField<bool>()));
    fpga.set_reconfiguration_slots(
        Array<FpgaReconfigurationSlot>::from_json(
            json.value(literals::Fpga::RECONFIGURATION_SLOTS, json::Json::array())
        )
    );
    fpga.set_erased(json.value(literals::Fpga::ERASED, OptionalField<bool>()));

    return fpga;
}
