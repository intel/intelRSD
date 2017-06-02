/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file status/module_software_status.hpp
 * @brief Module software status implementation
*/

#pragma once

#include "agent-framework/status/module_software_status.hpp"

/*! Agent namespace */
namespace agent {
/*! Pnc namespace */
namespace pnc {
/*! Status namespace */
namespace status {

/*! ModuleSoftwareStatus class. */
class ModuleSoftwareStatus : public agent_framework::status::ModuleSoftwareStatus {
public:
    /*! @brief Class destructor. */
    virtual ~ModuleSoftwareStatus();

    /*! Method reads Hardware Status of Module. */
    Status read_status();

};

}
}
}
