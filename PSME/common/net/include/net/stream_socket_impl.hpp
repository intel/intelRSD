//
// StreamSocketImpl.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/StreamSocketImpl.h#1 $
//
// Library: Net
// Package: Sockets
// Module:  StreamSocketImpl
//
// Definition of the StreamSocketImpl class.
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
 * @file stream_socket_impl.hpp
 *
 * @brief StreamSocket implementation
 * */

#pragma once

#include "socket_impl.hpp"

namespace net {

/*!
 * TCP stream socket implementation.
 */
class StreamSocketImpl : public SocketImpl {
public:
    /*!
     * Constructor
     */
    StreamSocketImpl();

    /*!
     * Constructor
     * @param address_family AddressFamily of created socket.
     */
    explicit StreamSocketImpl(AddressFamily address_family);

    /*!
     * Constructor
     * @param sockfd Socket descriptor used for object creation.
     */
    explicit StreamSocketImpl(net_socket_t sockfd);

    /*!
     * Ensures that all data in buffer is sent if the socket
     * is blocking. In case of a non-blocking socket, sends as
     * many bytes as possible.
     *
     * @return Number of bytes sent. The return value may also be
     * negative to denote some special condition.
     * */
    long send_bytes(const void* buffer, size_t length, int flags = 0) override;

    /*!
     * Destructor
     */
    virtual ~StreamSocketImpl();
};

} // namespace net
