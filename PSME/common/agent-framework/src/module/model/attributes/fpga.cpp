/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
    json::Json json{};

    json[literals::Fpga::TYPE] = m_type;
    json[literals::Fpga::BIT_STREAM_VERSION] = m_bit_stream_version;
    json[literals::Fpga::HSSI_CONFIGURATION] = m_hssi_configuration;
    json[literals::Fpga::HSSI_SIDEBAND] = m_hssi_sideband;
    json[literals::Fpga::RECONFIGURATION_SLOTS] = m_reconfiguration_slots;

    return json;
}

Fpga Fpga::from_json(const json::Json& json) {
    Fpga fpga{};

    fpga.set_type(json[literals::Fpga::TYPE]);
    fpga.set_bit_stream_version(json[literals::Fpga::BIT_STREAM_VERSION]);
    fpga.set_hssi_configuration(json[literals::Fpga::HSSI_CONFIGURATION]);
    fpga.set_hssi_sideband(json[literals::Fpga::HSSI_SIDEBAND]);
    fpga.set_reconfiguration_slots(json[literals::Fpga::RECONFIGURATION_SLOTS]);

    return fpga;
}
