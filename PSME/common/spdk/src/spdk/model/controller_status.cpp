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
 * @file controller_status.cpp
 */

#include "spdk/model/controller_status.hpp"



using namespace spdk;

constexpr const char model::ControllerStatus::RDY[];
constexpr const char model::ControllerStatus::CFS[];


void model::from_json(const json::Json& json, model::ControllerStatus& csts) {
    csts.set_cfs(json.at(model::ControllerStatus::CFS).get<std::uint64_t>());
    csts.set_rdy(json.at(model::ControllerStatus::RDY).get<std::uint64_t>());
}


void model::to_json(json::Json& json, const model::ControllerStatus& csts) {
    json = json::Json{
        {model::ControllerStatus::CFS, csts.get_cfs()},
        {model::ControllerStatus::RDY, csts.get_rdy()}
    };
}


std::string model::ControllerStatus::to_string() const {

    std::stringstream ss{};
    ss << "   CFS: " << get_cfs() << std::endl;
    ss << "   RDY: " << get_rdy() << std::endl;

    return ss.str();
}