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
 * @file command/storage/add_logical_drive.hpp
 *
 * @brief Generic storage AddLogicalDrive command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_LOGICAL_DRIVE_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_LOGICAL_DRIVE_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/logical_drive.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/enum/storage.hpp"

#include <vector>
#include <string>

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class AddLogicalDrive; }

using DriveArray = std::vector<std::string>;

/*! Generic storage command AddLogicalDrive */
class AddLogicalDrive : public Command {
public:

    /*! Tag std::string for identify agent */
    static constexpr char AGENT[] = "Storage";

    /*! Tag std::string for identify command */
    static constexpr char TAG[] = "addLogicalDrive";

    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::AddLogicalDrive;
        model::enums::LogicalDriveType m_type{
            model::enums::LogicalDriveType::LVM};
        double m_capacity_gb{};
        model::enums::LogicalDriveMode m_mode{
            model::enums::LogicalDriveMode::JBOD};
        std::string m_master{};
        std::string m_image{};
        bool m_snapshot{};
        bool m_protected{};
        DriveArray m_drives{};
        model::attribute::Oem m_oem{};
    public:

        /*!
         * @brief Get logical drive type
         * @return Logical drive type
         * */
        model::enums::LogicalDriveType get_type() const {
            return m_type;
        }

        /*!
         * @brief Get logical drive capacity
         * @return Logical drive capacity
         * */
        double get_capacity_gb() const {
            return m_capacity_gb;
        }

        /*!
         * @brief Get logical drive capacity in bytes
         * @return Logical drive capacity in bytes
         * */
        uint64_t get_capacity_bytes() const {
            return static_cast<uint64_t>(m_capacity_gb * 1000 * 1000 * 1000);
        }

        /*!
         * @brief Get logical drive creation mode
         * @return Logical drive mode
         * */
        model::enums::LogicalDriveMode get_mode() const {
            return m_mode;
        }

        /*!
         * @brief Get logical drive master UUID
         * @return Logical drive master UUID
         * */
        std::string get_master() const {
            return m_master;
        }

        /*!
         * @brief Get logical drive image
         * @return Logical drive image
         * */
         std::string get_image() const {
              return m_image;
          }

        /*!
         * @brief Indicates if drive drive should be created as a snapshot
         *  or as a full copy of the master drive
         * @return true if snapshot mode
         * */
        bool is_snapshot() const {
            return m_snapshot;
        }

        /*!
         * @brief Indicates if the logical drive should be marked
         * as write/delete protected after creation.
         * @return true if protected
         * */
        bool is_protected() const {
            return m_protected;
        }

        /*!
         * @brief The UUIDs of the drives (physical or logical)
         * that should be used to create the new logical drive on top
         *
         * @return Reference to vector of drives.
         * */
        std::vector<std::string>& get_drives() {
            return m_drives;
        }

        /*!
         * Get OEM specific data
         *
         * @return LogicalDrive OEM specific data
         * */
        model::attribute::Oem& get_oem() {
            return m_oem;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        /*!
         * @brief Set drive uuid
         * @param[in] uuid created Logical Drive uuid
         * */
        void set_drive_uuid(const std::string& uuid) {
            m_drive = uuid;
        }

        /*!
         * @brief Set OEM specific data for response
         * @param[in] oem OEM specific data
         * */
        void set_oem(const model::attribute::Oem& oem) {
            m_oem = oem;
        }


        ~Response();
    private:
        friend class json::AddLogicalDrive;
        std::string m_drive{};
        model::attribute::Oem m_oem{};
    };

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~AddLogicalDrive();
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
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_ADD_LOGICAL_DRIVE_HPP */
