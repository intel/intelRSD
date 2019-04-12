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

#include "agent-framework/module/network_components.hpp"
#include "utils/port.hpp"
#include "utils/lag.hpp"

#include <string>

using namespace agent_framework::module;
using namespace std;

namespace agent {
namespace network {
namespace utils {

bool get_port_uuid_by_identifier(const string& port, string& uuid) {
    if ("sw0p1" == port) {
        uuid = NetworkComponents::get_instance()->get_port_manager().get_keys().front();
        return true;
    }
    return false;
}

bool is_logical_port_empty(const string&) {
    return true;
}

bool is_member_port(const string&) {
    return false;
}

void init_switch_vlan_port(const string&) {
}

string add_logical_port(const string&, const string&) {
    return "";
}

}
}
}
