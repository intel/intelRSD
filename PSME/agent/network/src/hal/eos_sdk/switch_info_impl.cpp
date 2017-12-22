/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file switch_info_impl.cpp
 *
 * @brief Switch parameters
 * */

#include "hal/switch_info_impl.hpp"

using namespace agent::network::hal;

SwitchInfoImpl::SwitchInfoImpl(const std::string& mgmt_port) {
    set_is_enabled(true);
    set_management_port(mgmt_port);
}

SwitchInfoImpl::~SwitchInfoImpl() {}

void SwitchInfoImpl::read_switch_info() {
    read_mgmt_mac_address();
}

void SwitchInfoImpl::read_switch_port_list() {
}

void SwitchInfoImpl::add_switch_port(const PortIdentifier&, PortMode)
{
}

void SwitchInfoImpl::delete_switch_port(const PortIdentifier&)
{
}
