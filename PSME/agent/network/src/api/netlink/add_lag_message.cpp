/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file add_lag_message.cpp
 *
 * @brief Switch Add Lag configuration
 * */

#include "api/netlink/add_lag_message.hpp"

#include <net/if.h>
#include <linux/rtnetlink.h>

using namespace agent::network::api::netlink;

AddLagMessage::AddLagMessage(const std::string& lag) : LagMessage(lag, {}) {
    set_nlhdr_flags(NLM_F_CREATE | NLM_F_EXCL);
    set_nlhdr_type(RTM_NEWLINK);
}

AddLagMessage::~AddLagMessage() { }
