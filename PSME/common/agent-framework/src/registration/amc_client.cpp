/*!
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
 * @file amc_client.cpp
 * */

#include "agent-framework/registration/amc_client.hpp"
#include "agent-framework/registration/registration_request.hpp"
#include "agent-framework/service_uuid.hpp"

#include "json-rpc/connectors/http_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

using namespace agent_framework::generic;

AmcClient::AmcClient(const std::string& url)
    : m_url{url},
      m_connector{new json_rpc::HttpClientConnector(url)},
      m_invoker{new json_rpc::JsonRpcRequestInvoker()} {}

RegistrationResponse AmcClient::attach() const {
    RegistrationRequest request;

    m_invoker->prepare_method("attach", request.to_json());
    m_invoker->call(m_connector);
    json::Json result = m_invoker->get_result();

    if (result.is_object()) {
        log_debug("registration", "Registration response: " << result);
        RegistrationResponse response(result["ipv4address"], result["port"]);
        return response;
    }
    else {
        throw json_rpc::JsonRpcException(json_rpc::common::ERROR_CLIENT_INVALID_RESPONSE, result.dump());
    }
}

HeartBeatResponse AmcClient::heart_beat() const {
    json::Json request;
    request["gamiId"] = ServiceUuid::get_instance()->get_service_uuid();

    m_invoker->prepare_method("heartBeat", request);
    m_invoker->call(m_connector);
    json::Json result = m_invoker->get_result();

    if (result.is_object() && result.count("timeStamp") && result.count("minDelay")) {
        std::chrono::seconds delay(result["minDelay"]);
        std::chrono::seconds timestamp(result["timeStamp"]);
        return HeartBeatResponse(delay, timestamp);
    }
    else {
        throw json_rpc::JsonRpcException(json_rpc::common::ERROR_CLIENT_INVALID_RESPONSE, result.dump());
    }
}

AmcClient::~AmcClient() {}
