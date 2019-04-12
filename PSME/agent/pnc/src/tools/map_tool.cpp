/*!
 * @section LICENSE
 *
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
 * @file map_tool.cpp
 *
 * @brief MapTool implementation
 * */

#include "tools/map_tool.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::tools;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::nvme;

MapTool::~MapTool() {}

attribute::Status MapTool::get_port_status_from_link_status(const LinkStatusRetrieve& lsr) const {
    PCIeLinkState state = PCIeLinkState(std::uint32_t(lsr.output.fields.major));
    attribute::Status status{};
    status.set_health(enums::Health::OK);

    switch (state) {
        case PCIeLinkState::L0:
        case PCIeLinkState::RECOV:
        case PCIeLinkState::L1:
        case PCIeLinkState::L2:
            status.set_state(enums::State::Enabled);
            break;
        case PCIeLinkState::DETECT:
            status.set_state(enums::State::StandbyOffline);
            break;
        case PCIeLinkState::POLL:
        case PCIeLinkState::CFG:
            status.set_state(enums::State::Starting);
            break;
        case PCIeLinkState::DISABLE:
            status.set_state(enums::State::Disabled);
            break;
        case PCIeLinkState::LOOPBK:
        case PCIeLinkState::HOTRST:
        case PCIeLinkState::TXLOS:
            status.set_state(enums::State::InTest);
            break;
        default:
            status.set_health(enums::Health::Critical);
            status.set_state(enums::State::StandbyOffline);
            break;
    }
    return status;
}

std::tuple<uint32_t, uint32_t, double> MapTool::get_port_width_and_speed_from_link_status(
        const LinkStatusRetrieve lsr) const {
    uint32_t width = lsr.output.fields.neg_link_width;
    uint32_t max_width = lsr.output.fields.cfg_link_width;
    double speed = pcie_gen_to_transfer[PCIeGen(lsr.output.fields.linkup_link_rate & 0x7f)];
    return std::make_tuple(width, max_width, speed);
}

attribute::Status MapTool::get_status_from_smart(const Smart& smart) const {
    attribute::Status status{};
    status.set_state(enums::State::StandbyOffline);
    status.set_health(enums::Health::OK);

    // set status
    if (!smart.is_reset_required()) {
        if (!smart.is_drive_ready()) {
            // reset is not required and drive is not ready = starting
            status.set_state(enums::State::Starting);
        }
        else if (smart.is_drive_functional()) {
            // reset is not required and drive is functional = enabled
            status.set_state(enums::State::Enabled);
        }
    }
    // reset is required = StandbyOffline, but with anomalous health

    // set health
    if (smart.is_reset_required()) {
        // reset is required = Critical
        status.set_health(enums::Health::Critical);
    }
    else {
        if (smart.is_space_or_temperature_warning_flag_set()) {
            // reset is not required but known error flags are set = Warning
            status.set_health(enums::Health::Warning);
        }
        if (smart.is_any_other_warning_flag_set() || !smart.is_drive_functional()) {
            // reset is not required but other error flags are set OR drive is not functional = Critical
            status.set_health(enums::Health::Critical);
        }
    }

    return status;
}

double MapTool::gtps_to_gbps(double speed, uint32_t width) const {
    // encoding scaling factors: different for various pcie generations
    constexpr double RATIO_GEN1_GEN2 = 8.0/10.0;
    constexpr double RATIO_GEN3 = 128.0/130.0;
    // possible speed values: gen1=2.5, gen2=5, gen3=8
    double ratio = (speed < 8.0) ? RATIO_GEN1_GEN2 : RATIO_GEN3;
    return ratio * speed * static_cast<double>(width);
}
