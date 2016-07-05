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
 * @file ipmb/receiver_thread.hpp
 * @brief IPMI IPMB reciver thread.
 * */

#pragma once
#include <ipmb/utils.hpp>

#include <mutex>
#include <thread>

extern "C" {
#include <arpa/inet.h>
}

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

/*! IPMI IPMB messages receiver */
class ReceiverThread {
public:

    /*!
     * @brief Constructor
     * @param mux_port MUX port
     * */
    ReceiverThread(in_port_t mux_port) : m_mux_port(mux_port) {
    }

    /*! Start thread */
    void start();

    virtual ~ReceiverThread();
private:
    std::thread m_thread{};
    volatile bool m_is_running {false};

    void task();
    ::in_port_t m_mux_port{};
};

/*! Receiver thread unique pointer */
using ReceiverThreadUniquePtr = std::unique_ptr<ReceiverThread>;

}
}
}

