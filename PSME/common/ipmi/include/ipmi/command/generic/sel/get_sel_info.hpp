/*!
 * @brief Get SEL Info Request/Response.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_sel_info.hpp
 */

#pragma once

#include "ipmi/command/generic/sel/sel_defs.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <vector>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*! Request message for GetSelInfo command */
class GetSelInfo : public Request {
public:
    /*! Default constructor */
    GetSelInfo();

    /*! Copy constructor */
    GetSelInfo(const GetSelInfo&) = default;

    /*! Assignment operator */
    GetSelInfo& operator=(const GetSelInfo&) = default;

    /*! Destructor */
    virtual ~GetSelInfo();

    virtual const char* get_command_name() const override {
        return "GetSelInfo";
    }

private:
    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where request data will be packed.
     */
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*! Response message for GetSelInfo command */
class GetSelInfo : public Response {
public:
    /*! Default constructor */
    GetSelInfo();

    /*! Copy constructor */
    GetSelInfo(const GetSelInfo&) = default;

    /*! Assignment operator */
    GetSelInfo& operator=(const GetSelInfo&) = default;

    /*! Destructor */
    virtual ~GetSelInfo();


    SelTimestamp get_last_add_timestamp() const {
        return last_add_timestamp;
    }

    SelTimestamp get_last_erase_timestamp() const {
        return last_erase_timestamp;
    }

    /*!
     * @brief Get number of entries in SEL
     * @return number of events in the log
     */
    SelEntriesCount get_entries_count() const {
        return entries_count;
    }

    /*!
     * @brief Get free space in SEL
     * @return Free space in bytes
     */
    SelFreeSpace get_free_space() const {
        return free_space;
    }

    /*!
     * @brief Check if SEL reservation is available
     * @return true if commands must be executed with proper reservation
     */
    bool is_reserve_supported() const {
        return reserve_supported;
    }

    /*!
     * @brief Check if SEL events are allowed to be deleted
     * @return true if delete command is supported
     */
    bool is_delete_supported() const {
        return delete_supported;
    }

    /*!
     * @brief Check if some event were missed (not included in SEL)
     * @return true if not all events are in the log (ti. some are missed)
     */
    bool is_overflow() const {
        return overflow;
    }

    virtual const char* get_command_name() const override {
        return "GetSelInfo";
    }

private:
    /*!
     * @brief Custom command completion codes handling
     * @param completion_code to select exception to throw
     * @throws ResponseError type exception.
     */
    [[noreturn]] void throw_error_on_completion_code(CompletionCode completion_code) const override;

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be parsed and stored in the response,
     */
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;

    SelEntriesCount entries_count{};
    SelFreeSpace free_space{};
    SelTimestamp last_add_timestamp{};
    SelTimestamp last_erase_timestamp{};
    bool reserve_supported{};
    bool delete_supported{};
    bool overflow{};
};

}

}
}
}
