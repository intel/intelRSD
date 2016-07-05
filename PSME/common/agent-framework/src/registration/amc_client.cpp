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
 * @file amc_client.cpp
 * */

#include "agent-framework/registration/amc_client.hpp"
#include "agent-framework/registration/registration_request.hpp"
#include "agent-framework/registration/service_uuid.hpp"

#include <jsonrpccpp/client/connectors/httpclient.h>
#include <jsonrpccpp/client.h>

using namespace agent_framework::generic;

AmcClient::AmcClient(const std::string& url)
    : m_url{url},
      m_http_connector{new jsonrpc::HttpClient{m_url}},
      m_jsonrpc_client{new jsonrpc::Client{*m_http_connector}} {}

RegistrationResponse AmcClient::attach() const {
    RegistrationRequest request;

    Json::Value result = m_jsonrpc_client->CallMethod("attach", request.to_json());
    if (result.isObject()) {
        log_debug(GET_LOGGER("registration"),
                  "Registration response: " << result);
        RegistrationResponse response(result["ipv4address"].asString(),
                                  result["port"].asInt());
        return response;
    }
    else {
        throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
}

HeartBeatResponse AmcClient::heart_beat() const {
    Json::Value request;
    request["gamiId"] = ServiceUuid::get_instance()->get_service_uuid();

    Json::Value result = m_jsonrpc_client->CallMethod("heartBeat", request);
    if (result.isObject() && result.isMember("timestamp")
            && result.isMember("minDelay")) {
        HeartBeatResponse response(
                std::chrono::seconds(result["minDelay"].asUInt()),
                std::chrono::seconds(result["timestamp"].asUInt()));
        return response;
    }
    else {
        throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
}

AmcClient::~AmcClient() {}
