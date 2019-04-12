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
 * Unless msguired by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file get_sled_presence.cpp
 * @brief IPMI Get SLED Presence.
 * */

#include <ipmb/command/get_sled_presence.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>
#include <ipmb/gpio.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;

void GetSledPresence::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Get SLED Presence Command.");

    auto gpio = Gpio::get_instance();
    auto data = msg.get_data();

    if (data) {
        auto cc = *data;
        if (uint8_t(CompletionCode::CC_OK) != cc) {
            log_error(LOGUSR, "Cannot get SLED presence from MMP. CC=" << unsigned(cc));
            return;
        }

        auto presence = *(data + 1);
        if (presence & 0x1) {
            presence = static_cast<uint8_t>(presence >> 1);
            presence &= 0x0f;
            gpio->set_presence(presence);
        }
    }

    msg.set_to_response();
}

void GetSledPresence::pack(IpmiMessage& msg){

    /* Please take on account that GetSledPresence command is being used in
     * diffrent way than all other commands are. When the command is used, USB
     * device is working in master mode which means that PSME initializes
     * communication to MMP */

    msg.set_dest(I2C_MMP_ADDRESS);
    msg.set_netfn(uint8_t(NetFn::IPMI_NETFN_INTEL_REQ));
    msg.set_dlun(IPMI_MSG_DEFAULT_LUN);
    msg.set_src(I2C_SLAVE_ADDRESS);
    msg.set_seq(0);
    msg.set_slun(IPMI_MSG_OEM_REQ_LUN);
    msg.set_cmd(uint8_t(CmdCode::GET_SLED_PRESENCE));

    auto data = msg.get_data();
    data[OFFSET_CC] = uint8_t(CompletionCode::CC_OK);

    // This is total frame size. This one is ont updated by service.
    msg.add_len(IPMB_FRAME_HDR_LEN + CMD_REQUEST_DATA_LENGTH);

    msg.set_to_request();
}
