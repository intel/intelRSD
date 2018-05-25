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
 * */


#include "psme/rest/endpoints/simple_update.hpp"
#include "psme/rest/endpoints/simple_update_action_info.hpp"
#include "psme/rest/server/error/error_factory.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ActionInfo.ActionInfo";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#ActionInfo.v1_0_0.ActionInfo";

    r[ActionInfo::PARAMETERS] = json::Value::Type::ARRAY;
    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}


endpoint::SimpleUpdateActionInfo::SimpleUpdateActionInfo(const std::string& path) : EndpointBase(path) {}


endpoint::SimpleUpdateActionInfo::~SimpleUpdateActionInfo() {}


void endpoint::SimpleUpdateActionInfo::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    json::Value image_uri{};
    image_uri[Common::NAME] = constants::UpdateService::IMAGE_URI;
    image_uri[ActionInfo::REQUIRED] = true;
    image_uri[ActionInfo::DATA_TYPE] = "String";

    json::Value transfer_protocol{};
    transfer_protocol[Common::NAME] = constants::UpdateService::TRANSFER_PROTOCOL;
    transfer_protocol[ActionInfo::REQUIRED] = false;
    transfer_protocol[ActionInfo::DATA_TYPE] = "String";
    transfer_protocol[ActionInfo::ALLOWABLE_VALUES] = json::Value::Type::ARRAY;
    for (const auto& protocol : SimpleUpdate::get_allowed_protocols()) {
        transfer_protocol[ActionInfo::ALLOWABLE_VALUES].push_back(protocol.to_string());
    }

    json::Value targets{};
    targets[Common::NAME] = constants::UpdateService::TARGETS;
    targets[ActionInfo::REQUIRED] = true;
    targets[ActionInfo::DATA_TYPE] = "Array";

    r[ActionInfo::PARAMETERS].push_back(std::move(image_uri));
    r[ActionInfo::PARAMETERS].push_back(std::move(transfer_protocol));
    r[ActionInfo::PARAMETERS].push_back(std::move(targets));

    set_response(response, r);
}
