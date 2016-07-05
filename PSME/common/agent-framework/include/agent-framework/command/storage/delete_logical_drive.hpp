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
 * @file command/storage/delete_logical_drive.hpp
 * @brief Generic storage DeleteLogicalDrive command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_DELETE_LOGICAL_DRIVE_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_DELETE_LOGICAL_DRIVE_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/logical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"

#include <string>

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class DeleteLogicalDrive; }


/*! Generic storage command DeleteLogicalDrive */
class DeleteLogicalDrive : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr char AGENT[] = "Storage";

    /*! Tag string for identify command */
    static constexpr char TAG[] = "deleteLogicalDrive";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~DeleteLogicalDrive();
protected:
    /*!
     * @brief Execute command with givent command arguments
     *
     * @param[in]   in      Input command argument
     * @param[out]  out     Output command argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
public:
    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::DeleteLogicalDrive;
        std::string m_drive{};
    public:

        /*!
         * @brief Get the uuid of the logical drive to be deleted
         * @return Logical drive uuid
         * */
        std::string get_drive() const {
            return m_drive;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:

        /*!
         * @brief Set OEM specific data for response
         * @param[in] oem OEM specific data
         * */
        void set_oem(const model::attribute::Oem& oem) {
            m_oem = oem;
        }


        ~Response();
    private:
        friend class json::DeleteLogicalDrive;
        model::attribute::Oem m_oem{};
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_DELETE_LOGICAL_DRIVE_HPP */
