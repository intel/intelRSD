/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file ipmb/message_handler.cpp
 * @brief IPMI message handler.
 * */

#include <ipmb/patcher.hpp>
#include <ipmb/ipmi_message.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;

void Patcher::patch(const IpmiMessage& req,  IpmiMessage& rsp) {
    rsp.set_dest(req.get_src());
    rsp.set_netfn(req.get_netfn());
    rsp.set_dlun(req.get_slun());
    rsp.set_seq(req.get_seq());
    rsp.set_src(req.get_dest());
    rsp.set_slun(req.get_dlun());
    rsp.set_cmd(req.get_cmd());

    rsp.add_len(IPMB_FRAME_HDR_LEN + IPMB_CHCKSUM_LEN);

    log_debug(LOGUSR, "IPMI Message patched.");
}
