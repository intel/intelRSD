/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file host.cpp
 */

#include "spdk/model/host.hpp"



using namespace spdk;

constexpr const char model::Host::NQN[];


void model::from_json(const json::Json& json, model::Host& host) {
    if (json.is_string()) {
        host.set_nqn(json.get<std::string>());
    }
    else {
        host.set_nqn(json.at(model::Host::NQN).get<std::string>());
    }
}


void model::to_json(json::Json& json, const model::Host& host) {
    json = host.get_nqn();
}


std::string model::Host::to_string() const {

    std::stringstream ss{};
    ss << "   NQN: " << get_nqn() << std::endl;

    return ss.str();
}