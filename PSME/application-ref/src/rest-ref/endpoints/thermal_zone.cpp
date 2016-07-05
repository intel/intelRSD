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

#include "psme/rest-ref/endpoints/thermal_zone.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"

using namespace psme::rest;
using namespace app::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Thermal.Thermal";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Thermal.1.0.0.Thermal";
    r[Common::NAME] = "Thermal Zone";
    r[Common::DESCRIPTION] = "Thermal Zone description";
    r[Common::ID] = json::Value::Type::NIL;

    r[ThermalZone::FANS] = json::Value::Type::ARRAY;

    json::Value rs;
    rs[Common::ODATA_TYPE] = "Intel.Oem.Thermal";
    rs[ThermalZone::DESIRED_SPEED_RPM] = json::Value::Type::NIL;
    rs[ThermalZone::MAX_FANS_SUPPORTED] = json::Value::Type::NIL;
    rs[ThermalZone::PWM] = json::Value::Type::NIL;
    rs[ThermalZone::POLICY] = json::Value::Type::NIL;
    rs[Zone::RACK_LOCATION][Zone::UHEIGHT] = json::Value::Type::NIL;
    rs[Zone::RACK_LOCATION][Zone::ULOCATION] = json::Value::Type::NIL;
    rs[Zone::RACK_LOCATION][Zone::UNIT] = json::Value::Type::NIL;
    rs[Zone::RACK_LOCATION][Zone::XLOCATION] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    r[ThermalZone::TEMPERATURES] = json::Value::Type::ARRAY;

    return r;
}
}

endpoint::ThermalZone::ThermalZone(const std::string& path) : Endpoint(path) {}
endpoint::ThermalZone::~ThermalZone() {}

void endpoint::ThermalZone::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    set_response(res, json);
}
