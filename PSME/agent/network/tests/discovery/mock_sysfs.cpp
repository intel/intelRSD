/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#include "api/netlink/sysfs.hpp"

#include <vector>
#include <string>

using namespace agent::network::api::netlink;
using namespace std;

SysFs::~SysFs() {}

const vector<string> SysFs::get_port_list() const {
    vector<string> v;
    v.push_back("sw0p1");
    v.push_back("sw0p12");
    return v;
}

const vector<string> SysFs::get_team_list() const {
    vector<string> v;
    v.push_back("team1");
    return v;
}
