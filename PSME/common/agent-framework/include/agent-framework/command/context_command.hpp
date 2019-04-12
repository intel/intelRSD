/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file context_command.hpp
 *
 * @brief Command implementation
 * */

#pragma once

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <cassert>
#include <memory>

/*! Agent Framework */
namespace agent_framework {
/*! Command */
namespace command {

/*
 * @brief Generic interface of the JSON RPC commands
 * */
template <typename CONTEXT>
class ContextCommandBase {
public:
    virtual ~ContextCommandBase() {}

    /*
     * @brief Generic JSON RPC Method Call
     * @param[in] req request
     * @param[out] rsp response
     * */
    virtual void method(std::shared_ptr<CONTEXT> ctx, const json::Json& req, json::Json& rsp) = 0;


    /*
     * @brief Generic JSON RPC Notification Call
     * @param[in] req request
     * */
    virtual void notification(std::shared_ptr<CONTEXT> ctx, const json::Json& req) = 0;


    /*
     * @brief Returns scheme defining procedure format
     * @return Scheme of the procedure
     * */
    virtual const jsonrpc::ProcedureValidator& get_procedure() const = 0;
};

/*
 * @brief Generic implementation of the CommandBase based on the agent_framework model classes
 * */
template<typename CONTEXT, typename REQ, typename RSP>
class ContextCommand final : public ContextCommandBase<CONTEXT> {
public:

    using Request = REQ;
    using Response = RSP;
    using ContextPtr = std::shared_ptr<CONTEXT>;
    using HandlerFunc = std::function<void(std::shared_ptr<CONTEXT>, REQ, RSP&)>;


    ContextCommand(HandlerFunc handler) : m_handler(handler) {}

    /*
     * @brief Method handler implementation based on model request/response classes
     * @param ctx Context
     * @param[in] req request
     * @param[out] rsp response
     * */
    void method(std::shared_ptr<CONTEXT> ctx, const json::Json& req, json::Json& rsp) {
        Request request = Request::from_json(req);
        Response response{};
        m_handler(ctx, request, response);
        rsp = response.to_json();
    }


    /*
     * @brief Notification handler implementation based on model request classes
     * @param[in] req request
     * */
    void notification(std::shared_ptr<CONTEXT> ctx, const json::Json& req) {
        Request request = Request::from_json(req);
        Response response{};
        m_handler(ctx, request, response);
    }


    /*
     * @brief Returns JSON RPC command scheme
     * @return Scheme of the procedure, to be used by JSON RPC library
     * */
    const jsonrpc::ProcedureValidator& get_procedure() const {
        return Request::get_procedure();
    }


private:
    HandlerFunc m_handler{};

};

}
}
