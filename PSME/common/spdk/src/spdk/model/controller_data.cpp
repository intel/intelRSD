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
 * @file controller_data.cpp
 */

#include "spdk/model/controller_data.hpp"



using namespace spdk;

constexpr const char model::ControllerData::FIRMWARE_REVISION[];
constexpr const char model::ControllerData::SERIAL_NUMBER[];
constexpr const char model::ControllerData::VENDOR_ID[];
constexpr const char model::ControllerData::OACS[];
constexpr const char model::ControllerData::MODEL_NUMBER[];


void model::from_json(const json::Json& json, model::ControllerData& ctrlr) {
    ctrlr.set_firmware_revision(json.at(model::ControllerData::FIRMWARE_REVISION).get<std::string>());
    ctrlr.set_serial_number(json.at(model::ControllerData::SERIAL_NUMBER).get<std::string>());
    ctrlr.set_vendor_id(json.at(model::ControllerData::VENDOR_ID).get<std::string>());
    ctrlr.set_oacs(json.at(model::ControllerData::OACS).get<OptionalAdminCommandSupport>());
    ctrlr.set_model_number(json.at(model::ControllerData::MODEL_NUMBER).get<std::string>());
}


void model::to_json(json::Json& json, const model::ControllerData& ctrlr) {
    json = json::Json{
        {model::ControllerData::FIRMWARE_REVISION, ctrlr.get_firmware_revision()},
        {model::ControllerData::SERIAL_NUMBER,     ctrlr.get_serial_number()},
        {model::ControllerData::VENDOR_ID,         ctrlr.get_vendor_id()},
        {model::ControllerData::OACS,              ctrlr.get_oacs()},
        {model::ControllerData::MODEL_NUMBER,      ctrlr.get_model_number()}
    };
}


std::string model::ControllerData::to_string() const {

    std::stringstream ss{};
    ss << "   Firmawre Revision: " << get_firmware_revision() << std::endl;
    ss << "   Serial Number: " << get_serial_number() << std::endl;
    ss << "   Vendor ID: " << get_vendor_id() << std::endl;
    ss << "   Optional Admin Command Support: " << std::endl << get_oacs().to_string() << std::endl;
    ss << "   Model Number: " << get_model_number() << std::endl;

    return ss.str();
}