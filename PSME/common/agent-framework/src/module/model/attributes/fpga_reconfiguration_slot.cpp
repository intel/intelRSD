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
 * @file fpga_reconfiguration_slot.cpp
 * @brief Fpga slot attribute class implementation
 * */

#include "agent-framework/module/model/attributes/fpga_reconfiguration_slot.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/compute.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


json::Json FpgaReconfigurationSlot::to_json() const {
    json::Json json{};

    json[literals::FpgaReconfigurationSlot::SLOT_ID] = m_slot_id;
    json[literals::FpgaReconfigurationSlot::UUID] = m_uuid;
    json[literals::FpgaReconfigurationSlot::PROGRAMMABLE_FROM_HOST] = m_programmable_from_host;

    return json;
}


FpgaReconfigurationSlot FpgaReconfigurationSlot::from_json(const json::Json& json) {
    FpgaReconfigurationSlot fpga_slot{};

    fpga_slot.set_slot_id(json.value(literals::FpgaReconfigurationSlot::SLOT_ID, OptionalField<std::string>()));
    fpga_slot.set_uuid(json.value(literals::FpgaReconfigurationSlot::UUID, OptionalField<std::string>()));
    fpga_slot.set_programmable_from_host(json.value(literals::FpgaReconfigurationSlot::PROGRAMMABLE_FROM_HOST, OptionalField<bool>()));

    return fpga_slot;
}
