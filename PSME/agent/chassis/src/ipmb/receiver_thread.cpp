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
 *
 * @file service.cpp
 * @brief IPMI interface for RMM.
 * */

#include <ipmb/receiver_thread.hpp>
#include <ipmb/service.hpp>
#include <ipmb/utils.hpp>
#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;

ReceiverThread::~ReceiverThread(){
    m_is_running = false;
    m_thread.join();
    log_debug(LOGUSR, "ReceiverThread job done!");
}

void ReceiverThread::start() {
    if (!m_is_running) {
        m_thread = std::thread(&ReceiverThread::task, this);
        m_is_running = true;
    }
}

void ReceiverThread::task() {
    log_debug(LOGUSR, "IPMB Service ready.");
    auto service = Service::get_instance();
    static constexpr std::chrono::seconds NAP_TIME_SEC{2};

    while (m_is_running) {
        if (!service->is_initialized()) {
            if (!service->init(m_mux_port)) {  // try reconnecting
                std::this_thread::sleep_for(NAP_TIME_SEC);  // But delay reading thread before it tries again
                continue;
            }
        }
        IpmiMessage req{};
        if (service->slave_read(req)) {
            service->rcvd_msg_queue.push_back(req);
        }
    }
}


