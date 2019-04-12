/*!
 * @brief Request object to IPMI.
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
 * @file ipmi/request.hpp
 */

#pragma once



#include "message.hpp"

#include <cstdint>
#include <vector>



namespace ipmi {

class Request : public Message {
public:

    /*!
     * @brief Constructor.
     * @param fn Network Function (NetFn enum)
     * @param cmd Command
     */
    Request(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd);

    /*! Copy constructor */
    Request(const Request&) = default;

    /*! Move constructor */
    Request(Request&&) = default;

    /*! Assignment operator */
    Request& operator=(const Request&) = default;

    /*! Rvalue assignment operator */
    Request& operator=(Request&&) = default;

    /*!
     * @brief Destructor
     */
    virtual ~Request();


    /*!
     * @brief Packs data from Request object into ByteBuffer.
     * @return ByteBuffer with packed Request data.
     * */
    IpmiInterface::ByteBuffer do_pack() const;


private:
    /*!
     * @brief Packs data from Request object into vector of bytes.
     * @param data reference to vector where data will be stored.
     * */
    virtual void pack(IpmiInterface::ByteBuffer& data) const = 0;
};

}

