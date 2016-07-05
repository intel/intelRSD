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
 * @file command/compute/get_storage_controller_info.hpp
 * @brief Generic compute GetStorageControllerInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_STORAGE_CONTROLLER_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_STORAGE_CONTROLLER_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/storage_controller.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

namespace agent_framework {
namespace command {
namespace compute {

namespace json { class GetStorageControllerInfo; }

/*! Generic storage command GetStorageControllerInfo */
class GetStorageControllerInfo : public Command {
    public:
        /*! Tag string for identify agent */
        static constexpr const char AGENT[] = "Compute";

        /*! Tag string for identify CreateTarget command */
        static constexpr const char TAG[] = "getStorageControllerInfo";

        /*! Argument request to execute */
        class Request : public Argument {
            std::string m_controller{};
            friend class json::GetStorageControllerInfo;
        public:
            std::string get_controller() const {
                return m_controller;
            }

            ~Request();
        };

        /*! Argument response from execute */
        class Response : public Argument {
        public:
            /*!
             * Sets storage controller
             * @param[in] controller Storage controller
             * */
            void set_controller(const model::StorageController& controller) {
                m_controller = controller;
            }

            ~Response();
        private:
            friend class json::GetStorageControllerInfo;
            model::StorageController m_controller{};
        };

        /*!
         * @brief Execute command with given request and response argument
         *
         * @param[in]   request     Input request argument
         * @param[out]  response    Output response argument
         * */
        virtual void execute(const Request& request, Response& response) = 0;

        /*! Command destructor */
        virtual ~GetStorageControllerInfo();

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

#endif /*! AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_STORAGE_CONTROLLER_INFO_HPP*/
