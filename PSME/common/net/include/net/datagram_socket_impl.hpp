//
// DatagramSocketImpl.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/DatagramSocketImpl.h#1 $
//
// Library: Net
// Package: Sockets
// Module:  DatagramSocketImpl
//
// Definition of the DatagramSocketImpl class.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//
/*!
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
 *
 * @file datagram_socket_impl.hpp
 *
 * @brief DatagramSocket implementation
 * */

#pragma once

#include "socket_impl.hpp"

namespace net {

/*!
 * @brief This class implements datagram socket.
 */
class DatagramSocketImpl : public SocketImpl {
public:
    /*!
     * @brief Constructor.
     * Creates an unconnected, unbound datagram socket.
     */
    DatagramSocketImpl();

    /*!
     * @brief Constructor.
     * Creates an unconnected datagram socket for the given address family.
     * @param family AddressFamily of created socket.
     */
    explicit DatagramSocketImpl(AddressFamily family);

    /*!
     * @brief Constructor.
     * Creates a datagram socket for the given native socket.
     * @param sockfd Native socket descriptor.
     */
    DatagramSocketImpl(net_socket_t sockfd);

protected:
    void init(int af) override;

    ~DatagramSocketImpl();
};


} // namespace net
