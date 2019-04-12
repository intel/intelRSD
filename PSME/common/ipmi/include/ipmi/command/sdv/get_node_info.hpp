/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file get_node_info.hpp
 *
 * @brief ...
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Represents request message for Get Node Info command.
 */
class GetNodeInfo : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetNodeInfo();

    /*! Copy constructor. */
    GetNodeInfo(const GetNodeInfo&) = default;

    /*! Assignment operator */
    GetNodeInfo& operator=(const GetNodeInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetNodeInfo();

    const char* get_command_name() const override {
        return "GetNodeInfo";
    }

    virtual void pack(std::vector<std::uint8_t>& data) const override {
        // GetNodeInfo without request parameters.
        (void)data;
    }

private:

};
}

namespace response {

/*!
 * @brief Represents response message for Get Node Info command. Returns list of available nodes.
 */
class GetNodeInfo : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetNodeInfo();

    /*! Copy constructor. */
    GetNodeInfo(const GetNodeInfo&) = default;

    /*! Assignment operator */
    GetNodeInfo& operator=(const GetNodeInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetNodeInfo();

    const char* get_command_name() const override {
        return "GetNodeInfo";
    }

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

    /*!
     * @brief Checks if node is present (slot is occupied).
     * @param node_nr node number.
     * @return true if node exists, otherwise false.
     */
    bool is_present(std::size_t node_nr) const;

    /*!
     * @brief Gets number of available nodes.
     * @return number of available nodes.
     */
    std::size_t get_nodes_number() const;

private:
    std::uint8_t m_nodes_no{};
    std::vector<bool> m_nodes_present{};

    static constexpr std::size_t NODE_NUMBER = 12;
    static constexpr std::size_t OFFSET_NODES_NUMBER = 1;
    static constexpr std::size_t OFFSET_NODES_PRESENT = 2;
    static constexpr std::uint8_t NODE_PRESENT_MASK = 0x01;

    static constexpr std::size_t RESPONSE_SIZE = 14;
};
}

}
}
}
