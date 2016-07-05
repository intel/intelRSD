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
 * @file warm_reset.cpp
 * @brief WarmReset IPMI command handler for RMM.
 * */
#include <ipmb/command/warm_reset.hpp>

#include <logger/logger_factory.hpp>

#include <future>

extern "C" {
#include <unistd.h>
#include <errno.h>
#include <sys/reboot.h>
}

using namespace agent::chassis::ipmb::command;

WarmReset::~WarmReset() {}

namespace {
void execute_reset() {
    std::thread([] () {
        std::this_thread::sleep_for(
                std::chrono::seconds{CMD_REBOOT_TIMEOUT_SEC});
        sync();
        reboot(RB_AUTOBOOT);
    }).detach();
}
}

void WarmReset::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);

    msg.add_len(get_len());
}

void WarmReset::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking WarmReset message.");
    msg.set_to_request();
}

void WarmReset::pack(IpmiMessage& msg){

    log_debug(LOGUSR, "Packing WarmReset message.")

    m_response.set_cc(CompletionCode::CC_OK);

    msg.set_to_response();
    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);

    execute_reset();
}


