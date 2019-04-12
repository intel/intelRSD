/*!
 * @brief Implementation of Bmc class
 *
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
 * @file bmc.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "status/bmc.hpp"
#include "status/sensors_reader.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/utils/sdv/platform_discovery.hpp"

using namespace agent_framework::module;
using agent_framework::model::attribute::ConnectionData;
using ipmi::command::generic::BmcInterface;

namespace {

OptionalField<BmcInterface> read_inteface(agent::chassis::Bmc& bmc) {
    try {
        log_debug("bmc", bmc.get_id() << " reading sled interface");
        ipmi::command::generic::response::GetDeviceId device_rsp{};
        bmc.ipmi().send(ipmi::command::generic::request::GetDeviceId{}, device_rsp);
        auto interface = ipmi::sdv::platform_discovery(device_rsp);
        if (interface == BmcInterface::RSD_2_4) {
            return interface;
        }
        else if (interface == BmcInterface::RSA_1_2 || interface == BmcInterface::RSD_2_2) {
            log_error("bmc", "Unsupported legacy platform detected on " << bmc.get_id());
        }
        else {
            log_error("bmc", "Unknown platform detected on " << bmc.get_id());
        }
        return {};
    }
    catch (std::exception& e) {
        log_error("bmc", bmc.get_id() << " sled interface read failed: " << e.what());
    }
    return {};
}

}

namespace agent {
namespace chassis {

Bmc::Bmc(const std::string& manager_uuid, const ConnectionData& conn, Bmc::Duration state_update_interval,
         ReadPresenceFn read_presence, ReadOnlineStatusFn read_online_state)
    : agent_framework::Bmc(conn, state_update_interval, read_presence, read_online_state),
      m_ipmi{conn.get_ip_address(), conn.get_port(), conn.get_username(), conn.get_password()} {
    set_manager_uuid(manager_uuid);
}

bool Bmc::on_become_online(const Transition&) {
    m_interface = read_inteface(*this);
    get_manager<agent_framework::model::Manager>().get_entry_reference(get_manager_uuid())->set_presence(true);
    return true;
}

bool Bmc::on_extraction(const Transition&) {
    get_manager<agent_framework::model::Manager>().get_entry_reference(get_manager_uuid())->set_presence(false);
    m_interface.reset();
    return true;
}

bool Bmc::on_become_offline(const Transition& t) {
    if (Bmc::State::ONLINE == t.init_state) {
        get_manager<agent_framework::model::Manager>().get_entry_reference(get_manager_uuid())->set_presence(false);
    }
    return true;
}

void Bmc::reset_tasks() {
    agent_framework::Bmc::reset_tasks();
    using agent::chassis::SensorsReader;
    periodic_task("sensors_reader", std::chrono::seconds(0), std::chrono::seconds(3),
            std::bind(&SensorsReader::operator(), std::make_shared<SensorsReader>(get_manager_uuid(), std::ref(*this))));
}

}
}
