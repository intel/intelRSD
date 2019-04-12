/*!
 * @brief Switch Port Info class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file agent/network/src/hal/eos_eapi/switch_port_info_impl.cpp
 */

#include "hal/switch_port_info_impl.hpp"
#include "hal/eos_eapi/eapi_client.hpp"
#include "hal/eos_eapi/get_port_running_configuration.hpp"
#include "hal/eos_eapi/port_running_configuration_parser.hpp"
#include "hal/eos_eapi/set_port_pfc_enabled.hpp"
#include "hal/eos_eapi/set_port_pfc_priorities.hpp"
#include "hal/eos_eapi/set_port_lldp_enabled.hpp"
#include "hal/eos_eapi/set_port_dcbx_state.hpp"


using namespace agent::network::hal;

void SwitchPortInfoImpl::set_switch_port_pfc_enabled(bool state) {
    SetPortPfcEnabled command{get_port_identifier(), state};

    const auto response = EapiClient::get_instance()->eapi_request(command);
}

void SwitchPortInfoImpl::set_port_pfc_priorities(const std::vector<std::uint32_t>& to_set,
                                                 const std::vector<std::uint32_t>& to_remove) {
    SetPortPfcPriorities command{get_port_identifier(), to_set, to_remove};

    const auto response = EapiClient::get_instance()->eapi_request(command);
}

void SwitchPortInfoImpl::set_switch_port_lldp_enabled(bool state) {
    SetPortLldpEnabled command{get_port_identifier(), state};

    const auto response = EapiClient::get_instance()->eapi_request(command);
}

void SwitchPortInfoImpl::set_switch_port_dcbx_state(DcbxState dcbx_state) {
    SetPortDcbxState command{get_port_identifier(), dcbx_state};

    const auto response = EapiClient::get_instance()->eapi_request(command);
}

/* deprecated since the values are read during discovery
 * to be removed later
 * */
void SwitchPortInfoImpl::get_switch_port_pfc_enabled() {
    GetPortRunningConfiguration command{get_port_identifier()};

    // @TODO consider returning only the last element of the result table
    const auto response = EapiClient::get_instance()->eapi_request(command);

    // @TODO come up with a better name or mechanism
    eapi::PortRunningConfigurationParser port_config(response.dump());
    set_pfc_enabled(port_config.is_pfc_enabled());

    //@TODO set_enables_priorities(port_config.get_enabled_priorities());
}
