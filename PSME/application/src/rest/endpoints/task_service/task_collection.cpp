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
 * */

#include "psme/rest/endpoints/task_service/task_collection.hpp"
#include "psme/rest/utils/status_helpers.hpp"

#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#TaskCollection.TaskCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#TaskCollection.TaskCollection";
    r[Common::NAME] = "Task Collection";
    r[Common::DESCRIPTION] = "Task Collection";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}

namespace psme {
namespace rest {
namespace endpoint {

TaskCollection::TaskCollection(const std::string& path) : EndpointBase(path) { }


TaskCollection::~TaskCollection() { }


void TaskCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto& tasks_ids = get_manager<agent_framework::model::Task>().get_ids();
    json[Collection::ODATA_COUNT] = std::uint32_t(tasks_ids.size());

    for (const auto& id : tasks_ids) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}

}
}
}
