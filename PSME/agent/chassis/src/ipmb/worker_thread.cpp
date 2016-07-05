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

#include <ipmb/worker_thread.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/dispatcher.hpp>
#include <ipmb/patcher.hpp>
#include <ipmb/service.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;

WorkerThread::~WorkerThread(){
    m_is_running = false;
    m_thread.join();
    log_debug(LOGUSR, "WorkerThread job done!");
}

void WorkerThread::start() {
    if (!m_is_running) {
        m_thread = std::thread(&WorkerThread::task, this);
        m_is_running = true;
    }
}

void WorkerThread::task() {
    Dispatcher dispatcher{};
    Patcher patcher{};

    log_debug(LOGUSR, "IPMB worker thread is ready.");
    while (m_is_running) {
        IpmiMessage req{};
        IpmiMessage rsp{};
        auto service = Service::get_instance();

        service->rcvd_msg_queue.wait_and_pop(req);

        auto command = dispatcher.dispatch(req);
        command->unpack(req);
        if (req.is_request()) {
            command->pack(rsp);
            patcher.patch(req, rsp);
            service->master_write(rsp);
        }
    }
    log_debug(LOGUSR, "Stopping IPMB worker thread...");

}
