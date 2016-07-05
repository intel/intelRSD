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
 * @file command/storage/get_physical_drive_info.hpp
 * @brief Generic storage GetPhysicalDriveInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_GET_PHYSICAL_DRIVE_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_GET_PHYSICAL_DRIVE_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/physical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"

#include <string>

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class GetPhysicalDriveInfo; }

/*! Generic storage command GetPhysicalDriveInfo */
class GetPhysicalDriveInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr char AGENT[] = "Storage";

    /*! Tag string for identify command */
    static constexpr char TAG[] = "getPhysicalDriveInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetPhysicalDriveInfo();
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
        friend class json::GetPhysicalDriveInfo;
        std::string m_drive{};
    public:
        /*!
         * @brief Get drive UUID
         * @return Drive UUID
         * */
        const std::string& get_drive() const {
            return m_drive;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::GetPhysicalDriveInfo;
        model::PhysicalDrive m_drive{};
    public:

        /*!
         * @brief Set drive
         * @param[in] drive PhysicalDrive drive
         * */
        void set_drive(const model::PhysicalDrive& drive) {
            m_drive = drive;
        }

        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_GET_PHYSICAL_DRIVE_INFO_HPP */
