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
 * @file binding_info.hpp
 * @brief Binding Info Command
 * */

#pragma once

#include "gas/access_interface.hpp"
#include "gas/utils.hpp"

#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {
/*! MRPC namespace */
namespace mrpc {

class AccessInterafce;

using namespace agent::pnc::gas;



/*! MRPC Command interface */
class Command {
protected:
    AccessInterface* m_iface;
    CommandCode m_command{};
public:

    /*!
     * @brief Default constructor
     * @param iface Access interface pointer
     * @param command Code
     * */
    explicit Command(AccessInterface* iface, CommandCode command):
            m_iface{iface}, m_command(command) { }

    /*! Copy constructor */
    Command(const Command&) = default;

    /*! Assignment operator */
    Command& operator=(const Command&) = default;

    /*! Write input data */
    virtual void write_input() = 0;

    /*! Run command */
    void run();

    /*!
     * @brief Return command status
     * @return Command status
     * */
    CommandStatus read_status();

    /*! Read output data */
    virtual void read_output() = 0;

    virtual ~Command();
};

}
}
}
}
