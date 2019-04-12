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
 *
 * @file access_interface_factory.cpp
 * @brief GAS access interface factory implementation
 * */

#include "gas/access_interface_factory.hpp"
#include "gas/pcie_access_interface.hpp"

using namespace agent::pnc::gas;

AccessInterfaceFactory::~AccessInterfaceFactory() {}

AccessInterface* AccessInterfaceFactory::get_interface() {

    switch (GAS_CURRENT_INTERFACE) {
        case InterfaceCode::Pcie:
            return PcieAccessInterface::get_instance();
        case InterfaceCode::Uart:
            break;
        case InterfaceCode::Twi:
            break;
        default:
            break;
    }

    return nullptr;
}
