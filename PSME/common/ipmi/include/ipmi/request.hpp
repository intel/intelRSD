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
 *
 * @file request.hpp
 *
 * @brief Request object from IPMI.
 * */

#pragma once

#include "message.hpp"

#include <cstdint>
#include <vector>

namespace ipmi {

/*!
 * @brief Request object to IPMI.
 * */
class Request : public Message {
public:

    /*!
     * @brief Constructor.
     * @param fn Network Function (NetFn enum)
     * @param cmd Command
     */
    Request(NetFn fn, Cmd cmd);

    /*! Copy constructor */
    Request(const Request&) = default;

    /*! Assignment operator */
    Request& operator=(const Request&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~Request();

    /*!
     * @brief Packs data from Request object into vector of bytes.
     * @param data reference to vector where data will be stored.
     * */
    virtual void pack(std::vector<std::uint8_t>& data) const = 0;
};

}

