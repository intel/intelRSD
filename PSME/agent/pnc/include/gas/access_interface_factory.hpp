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
 * @file access_interface_factory.hpp
 * @brief GAS access interface factory
 * */

#pragma once

#include "gas/access_interface.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

/*! GAS access interface enum codes */
enum class InterfaceCode : std::uint8_t {
    Pcie = 0,
    Uart = 1,
    Twi = 2
};

/*! Specifies current GAS access interfaces */
static constexpr InterfaceCode GAS_CURRENT_INTERFACE = InterfaceCode::Pcie;

/*! GAS access interface factory */
class AccessInterfaceFactory {
public:
    /*! Default constructor */
    AccessInterfaceFactory() {}

    /*!
     * @brief Gets GAS access interface
     * @return GAS Access Interface
     * */
    AccessInterface* get_interface();

    virtual ~AccessInterfaceFactory();
};

}
}
}
