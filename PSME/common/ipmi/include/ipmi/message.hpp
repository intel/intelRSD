/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file message.hpp
 *
 * @brief ...
 * */

#ifndef IPMI_MESSAGE_HPP
#define IPMI_MESSAGE_HPP

#include <cstdint>

using std::uint8_t;
using Cmd = std::uint8_t;
using NetFn = std::uint8_t;

namespace ipmi {

/*!
 * @brief Represents Message used to communicate through IPMI. It's a base class for Request and Response.
 */
class Message {
public:
    /*!
     * @brief Constructor.
     * @param fn Network function.
     * @param cmd Command.
     */
    Message(uint8_t fn, Cmd cmd);

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
    void set_network_function(uint8_t fn);

    /**
     * Gets Network Function code.
     * @return NetFn
     */
    uint8_t get_network_function() const;

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
    uint8_t network_function {};
    Cmd command_code {};
};

}

#endif	/* IPMI_MESSAGE_HPP */

