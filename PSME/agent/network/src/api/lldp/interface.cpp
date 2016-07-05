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
 *
 * @file lldp_interface.cpp
 *
 * @brief LLDP communication interface.
 * */

#include "api/lldp/interface.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include "api/lldp/lldpad.hpp"

#include <sstream>
#include <memory>

using std::string;
using namespace agent::network::api::lldp;

Interface::Interface() : mp_conn{nullptr} {
    /* init LLDP client library */
    mp_conn = clif_open();
}

Response Interface::send(const Request& request) {
    std::unique_ptr<char[]> buff{new char[MAX_CLIF_MSGBUF]};
    size_t received = MAX_CLIF_MSGBUF;
    auto response = buff.get();

    /* sanity check */
    if (nullptr == mp_conn) {
        throw exception::ApiError(exception::ErrCode::NO_CONNECTION,
            "LLDP agent connection is closed");
    }

    /* send request to the LLDP agent daemon */
    const auto& req_msg = request.get_message();
    int rc = clif_request(mp_conn, req_msg.c_str(),
            req_msg.length(), response, &received, NULL);
    if (rc < 0) {
        throw exception::ApiError("clif_request() send request failed");
    }

    /* return response */
    return Response{{response, received}};
}

Interface::~Interface() {
    if (mp_conn) {
        clif_detach(mp_conn);
        clif_close(mp_conn);
    }
}
