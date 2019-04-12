/*!
 * @brief Definition of NetworkDeviceFunction model class
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file network_device_function.cpp
 */

#include "agent-framework/module/model/network_device_function.hpp"
#include "agent-framework/module/constants/compute.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component NetworkDeviceFunction::component = enums::Component::NetworkDeviceFunction;
const enums::CollectionName NetworkDeviceFunction::collection_name =
    enums::CollectionName::NetworkDeviceFunctions;


NetworkDeviceFunction::NetworkDeviceFunction(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


NetworkDeviceFunction::~NetworkDeviceFunction() {}


json::Json NetworkDeviceFunction::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkDeviceFunction::STATUS] = get_status().to_json();
    result[literals::NetworkDeviceFunction::DEVICE_ENABLED] = m_device_enabled;
    result[literals::NetworkDeviceFunction::ETHERNET][literals::NetworkDeviceFunction::MAC_ADDRESS] = m_mac_address;
    result[literals::NetworkDeviceFunction::ISCSI_BOOT] = m_iscsi_boot.to_json();
    result[literals::NetworkDeviceFunction::OEM] = get_oem().to_json();
    return result;
}


NetworkDeviceFunction NetworkDeviceFunction::from_json(const json::Json& json) {
    NetworkDeviceFunction function;
    function.set_status(attribute::Status::from_json(json[literals::NetworkDeviceFunction::STATUS]));
    function.set_device_enabled(json[literals::NetworkDeviceFunction::DEVICE_ENABLED]);
    if (json.count(literals::NetworkDeviceFunction::ETHERNET)
        && json[literals::NetworkDeviceFunction::ETHERNET].is_object()) {

        function.set_mac_address(
            json[literals::NetworkDeviceFunction::ETHERNET][literals::NetworkDeviceFunction::MAC_ADDRESS]);
    }
    function.set_iscsi_boot(attribute::IscsiBoot::from_json(json[literals::NetworkDeviceFunction::ISCSI_BOOT]));
    function.set_oem(attribute::Oem::from_json(json[literals::NetworkDeviceFunction::OEM]));

    return function;
}
