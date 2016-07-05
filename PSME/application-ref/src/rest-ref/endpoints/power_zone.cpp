/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/endpoints/power_zone.hpp"
#include "psme/rest-ref/constants/constants.hpp"

using namespace psme::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Power.Power";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Power.1.0.0.Power";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Power Zone";
    r[Common::DESCRIPTION] = "Power Zone description";

    r[PowerZone::POWER_CONTROL] = json::Value::Type::ARRAY;

    r[PowerZone::POWER_SUPPLIES] = json::Value::Type::ARRAY;

    return r;
}
}

endpoint::PowerZone::PowerZone(const std::string& path) : Endpoint(path) {}
endpoint::PowerZone::~PowerZone() {}

void endpoint::PowerZone::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();

    set_response(res, r);
}
