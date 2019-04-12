/*!
 * @brief Network device builder class implementation.
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
 * @file network_device_builder.cpp
 */

#include "discovery/builders/network_device_builder.hpp"
#include "agent-framework/module/model/network_device_function.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


NetworkDevice NetworkDeviceBuilder::build_default(const std::string& parent_uuid) {
    NetworkDevice network_device{parent_uuid};

    network_device.add_collection({enums::CollectionName::NetworkDeviceFunctions,
                                   enums::CollectionType::NetworkDeviceFunctions
    });

    return network_device;
}

void NetworkDeviceBuilder::update_from_function(NetworkDevice& device, const NetworkDeviceFunction& ndf) {
    device.set_status(ndf.get_status());
}
