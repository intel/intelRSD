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
 * @file request.hpp
 *
 * @brief Request object from IPMI.
 * */

#ifndef IPMI_REQUEST_HPP
#define IPMI_REQUEST_HPP

#include "message.hpp"

#include <cstdint>
#include <vector>

using std::uint8_t;
using std::vector;
using Cmd = std::uint8_t;
using NetFn = std::uint8_t;

namespace ipmi {

/*!
 * @brief Request object to IPMI.
 * */
class Request: public Message {
public:

    /*!
     * @brief Constructor.
     * @param fn Network Function (NetFn enum)
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

    /**
     * Packs data from Request object into vector of bytes.
     *
     * @param data reference to vector where data will be stored.
     */
    virtual void pack(vector<uint8_t> & data) const = 0;
};

}

#endif	/* IPMI_REQUEST_HPP */
