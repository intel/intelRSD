/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file ipmb/command/command_factory.hpp
 * @brief IPMI Command factory.
 * */

#pragma once
#include <ipmb/command/command.hpp>
#include <ipmb/utils.hpp>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {
/*! Command namespace */
namespace command {

/*! IPMI Command Factory */
class CommandFactory {
public:
    /*!
     * Get IPMI Command
     * @param lun IPMI Destination LUN
     * @param netfn IPMI Net Function
     * @param cmd_code IPMI Command
     * @return Pointer to IPMI Command
     * */
    CommandUniquePtr get_command(const uint8_t lun,
           const NetFn netfn, const CmdCode cmd_code);

};

}
}
}
}

