/*!
 * @brief Represents Message used to communicate through IPMI.
 *
 * It's a base class for Request and Response.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file ipmi/message.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"

#include <vector>
#include <cstdint>

namespace ipmi {

class Message {
public:
    static constexpr IpmiInterface::Lun CFG_LUN_DEFAULT = 0x00;

    /*!
     * @brief Constructor.
     * @param fn Network function.
     * @param cmd Command.
     */
    Message(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd);

    /*! Copy constructor */
    Message(const Message&) = default;

    /*! Move constructor */
    Message(Message&&) = default;

    /*! Assignment operator */
    Message& operator=(const Message&) = default;

    /*! Rvalue assignment operator */
    Message& operator=(Message&&) = default;

    /*!
     * Default destructor.
     */
    virtual ~Message();

    /*!
     * @brief Command name
     * @return Command name
     */
    virtual const char* get_command_name() const = 0;

    /*!
     * Sets Network Function for command.
     *
     * @param fn Network Function code
     */
    void set_network_function(IpmiInterface::NetFn fn);

    /*!
     * Gets Network Function code.
     * @return NetFn
     */
    IpmiInterface::NetFn get_network_function() const;

    /*!
     * Sets Command code.
     * @param cmd Command code.
     */
    void set_command(IpmiInterface::Cmd cmd);

    /*!
     * Gets Command code
     * @return command code.
     */
    IpmiInterface::Cmd get_command() const;

    /*!
     * Sets logical unit number
     * @param lun logical unit number
     */
    void set_lun(IpmiInterface::Lun lun);

    /*!
     * Gets logical unit number
     * @return logical unit number.
     */
    IpmiInterface::Lun get_lun() const;

private:
    IpmiInterface::NetFn network_function{};
    IpmiInterface::Cmd command_code{};
    IpmiInterface::Lun lun{CFG_LUN_DEFAULT};
};

}


