//
// NetException.cpp
//
// $Id: //poco/1.4/Net/src/NetException.cpp#4 $
//
// Library: Net
// Package: NetCore
// Module:  NetException
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
 * @file net_exception.cpp
 *
 * @brief NetException interface
 * */

#include "net/net_exception.hpp"

using namespace net;

NetException::NetException(const std::string& msg, int code)
    : std::runtime_error(msg), m_code(code) {}

NetException::~NetException() {}

InvalidSocketException::InvalidSocketException()
    : NetException("Invalid socket") {}

InvalidSocketException::~InvalidSocketException() {}

TimeoutException::TimeoutException(const std::string& msg, int code)
    : NetException(msg, code) {}

TimeoutException::~TimeoutException() {}
