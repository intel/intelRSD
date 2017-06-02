/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file message.hpp
 * @brief Base class for IPMI message.
 * */

#pragma once

#include <cstdint>

namespace ipmi {

/*!
 * @brief Represents Message used to communicate through IPMI.
 * It's a base class for Request and Response.
 */
class Message {
public:
    using Cmd = std::uint8_t;
    using NetFn = std::uint8_t;

    /*!
     * @brief Constructor.
     * @param fn Network function.
     * @param cmd Command.
     */
    Message(NetFn fn, Cmd cmd);

    /*! Copy constructor */
    Message(const Message&) = default;

    /*! Assignment operator */
    Message& operator=(const Message&) = default;

    /*!
     * Default destructor.
     */
    virtual ~Message();

    /*!
     * Sets Network Function for command.
     *
     * @param fn Network Function code
     */
    void set_network_function(NetFn fn);

    /*!
     * Gets Network Function code.
     * @return NetFn
     */
    NetFn get_network_function() const;

    /*!
     * Sets Command code.
     * @param cmd Command code.
     */
    void set_command(Cmd cmd);

    /*!
     * Gets Command code
     * @return command code.
     */
    Cmd get_command() const;

private:
    NetFn network_function {};
    Cmd command_code {};
};

}


