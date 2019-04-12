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
 * @file ipmb/command/get_sled_presence.hpp
 * @brief GetSledPresence IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>

#include <vector>

/*! Agent namesapce */
namespace agent {
/*! Chassis namesapce */
namespace chassis {
/*! IPMB namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetSledPresence command */
class GetSledPresence final : public Command {
public:
    /*! Default constructor */
    GetSledPresence(){}
    /*! Copy constructor */
    GetSledPresence(const GetSledPresence&) = default;
    /*! Assigment construcor */
    GetSledPresence& operator=(const GetSledPresence&) = default;

    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage& msg);

    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage& msg);

private:
    static const constexpr uint8_t CMD_REQUEST_DATA_LENGTH = 1;
    static const constexpr uint8_t OFFSET_CC = 0;
};

}
}
}
}

