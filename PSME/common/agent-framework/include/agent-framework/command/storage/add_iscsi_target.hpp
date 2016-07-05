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
 * @file command/storage/add_iscsi_target_info.hpp
 * @brief Generic storage AddIscsiTarget command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_ISCSI_TARGET_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_ISCSI_TARGET_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/iscsi_target.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class AddIscsiTarget; }

using TargetLunArray = model::attribute::Array<model::attribute::TargetLun>;

/*! Generic storage command AddIscsiTarget */
class AddIscsiTarget : public Command {
    public:
        /*! Tag string for identify agent */
        static constexpr const char AGENT[] = "Storage";

        /*! Tag string for identify AddIscsiTarget command */
        static constexpr const char TAG[] = "addiSCSITarget";

        /*! Argument request to execute */
        class Request : public Argument {
            friend class json::AddIscsiTarget;
            std::string m_initiator_iqn{};
            std::string m_target_iqn{};
            TargetLunArray m_target_luns{};
            model::attribute::Oem m_oem{};
        public:
            /*!
             * @brief Get target luns
             * @return Target luns
             */
            const TargetLunArray& get_target_luns() const {
                return m_target_luns;
            }

            /*!
             * @brief Get target iqn
             * @return Target iqn
             */
            const std::string& get_target_iqn() const {
                return m_target_iqn;
            }

            /*!
             * @brief Get initiator iqn
             * @return Initiator iqn
             */
            const std::string& get_initiator_iqn() const {
                return m_initiator_iqn;
            }

            /*!
             * @brief Get OEM data
             * @return OEM data object
             */
            const model::attribute::Oem& get_oem() const {
                return m_oem;
            }

            ~Request();
        };

        /*! Argument response from execute */
        class Response : public Argument {
            friend class json::AddIscsiTarget;
            std::string m_target{};
            model::attribute::Oem m_oem{};

        public:
            /*!
             * @brief Set target uuid
             * @param target Target uuid
             */
            void set_target(const std::string& target) {
                m_target = target;
            }

            /*!
             * @brief Set oem data
             * @param oem OEM object
             */
            void set_oem(const model::attribute::Oem& oem) {
                m_oem = oem;
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
        virtual ~AddIscsiTarget();

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

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_ISCSI_TARGET_HPP */
