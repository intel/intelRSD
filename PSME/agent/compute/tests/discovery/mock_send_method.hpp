/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#ifndef AGENT_COMPUTE_TESTS_MOCK_SEND_METHOD_HPP
#define	AGENT_COMPUTE_TESTS_MOCK_SEND_METHOD_HPP

#include "ipmi/management_controller.hpp"

/*! This class is the mock ManagmentController
 *  it provids the send method with the runtime error exception.
 */
class MockSendMethod: public ipmi::ManagementController {
public:
    /*! Default constructor */
    MockSendMethod() = default;
    /*! Default destrutor */
    virtual ~MockSendMethod();

    virtual void send(const ipmi::Request&, ipmi::Response&) {
        throw std::runtime_error("Runtime exception of send method.");
    }

    bool is_connected() const {
        return true;
    }
};

// To avoid vtable warning.
MockSendMethod::~MockSendMethod(){}

#endif	/* AGENT_COMPUTE_TESTS_MOCK_SEND_METHOD_HPP */
