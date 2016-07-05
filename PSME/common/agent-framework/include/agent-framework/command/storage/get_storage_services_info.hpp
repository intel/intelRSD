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
 * @file command/storage/get_storage_services_info.hpp
 * @brief Generic storage GetStorageServicesInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_GET_STORAGE_SERVICES_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_GET_STORAGE_SERVICES_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/storage_services.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"

#include <vector>

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class GetStorageServicesInfo; }

/*! Generic storage command GetStorageServicesInfo */
class GetStorageServicesInfo : public Command {
public:
    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Storage";

    /*! Tag string for identify GetStorageServicesInfo command */
    static constexpr const char TAG[] = "getStorageServicesInfo";

    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::GetStorageServicesInfo;
        std::string m_services{};
    public:
        /*!
         * @brief Get storage services instance UUID
         * @return Storage services instance UUID
         * */
        const std::string& get_services() const {
            return m_services;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        model::StorageServices m_services{};
        friend class json::GetStorageServicesInfo;
    public:
        /*! Default class constructor */
        Response() {}

        /*!
         * @brief Set Services for response
         * @param[in] services Services
         * */
        void set_services(const model::StorageServices& services) {
            m_services = services;
        }

        ~Response();
    };
    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetStorageServicesInfo();

protected:

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   in          Input request argument
     * @param[out]  out         Output response argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_GET_STORAGE_SERVICES_INFO_HPP */
