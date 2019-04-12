/*!
 * @brief UDev interface reader implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file udev_interface_reader.cpp
 */

#include "interface-reader/udev_interface_reader.hpp"
#include "udev/udev.hpp"
#include "logger/logger.hpp"

using namespace interface_reader;
using namespace net_udev;

UdevInterfaceReader::~UdevInterfaceReader() {}

std::vector<std::string> UdevInterfaceReader::get_interfaces() const {
    auto result = std::vector<std::string>{};

    UDev udev{};
    for (const auto& driver_name : m_drivers) {
        log_debug("interface-reader", "Looking for network interfaces by driver " << driver_name);
        auto interfaces = udev.get_interface_list(driver_name, "");
        for (const auto& interface : interfaces) {
            log_debug("interface-reader", "Discovered network interface " << interface);
            result.emplace_back(interface);
        }
    }

    return result;
}

