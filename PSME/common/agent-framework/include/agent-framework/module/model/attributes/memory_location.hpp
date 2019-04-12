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
 * @file memory_location.hpp
 * @brief MemoryLocation attribute
 * */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! MemoryLocation */
class MemoryLocation {
public:

    explicit MemoryLocation();

    MemoryLocation(std::uint32_t socket,
                 std::uint32_t controller,
                 std::uint32_t channel,
                 std::uint32_t slot):
                 m_socket{socket},
                 m_controller{controller},
                 m_channel{channel},
                 m_slot{slot}
                 {}

    ~MemoryLocation();

    /*! Enable copy */
    MemoryLocation(const MemoryLocation&) = default;
    MemoryLocation& operator=(const MemoryLocation&) = default;
    MemoryLocation(MemoryLocation&&) = default;
    MemoryLocation& operator=(MemoryLocation&&) = default;

    /*!
     * @brief Sets socket
     * @param[in] socket Socket
     * */
    void set_socket(const OptionalField<std::uint32_t>& socket) {
        m_socket = socket;
    }

    /*!
     * @brief Gets socket
     * @return Socket
     * */
    OptionalField<std::uint32_t> get_socket() const {
        return m_socket;
    }

    /*!
     * @brief Sets controller
     * @param[in] controller Controller
     * */
    void set_controller(const OptionalField<std::uint32_t>& controller) {
        m_controller = controller;
    }

    /*!
     * @brief Gets controller
     * @return Controller
     * */
    OptionalField<std::uint32_t> get_controller() const {
        return m_controller;
    }

    /*!
     * @brief Sets channel
     * @param[in] channel Channel
     * */
    void set_channel(const OptionalField<std::uint32_t>& channel) {
        m_channel = channel;
    }

    /*!
     * @brief Gets channel
     * @return Channel
     * */
    OptionalField<std::uint32_t> get_channel() const {
        return m_channel;
    }

    /*!
     * @brief Sets slot
     * @param[in] slot Slot
     * */
    void set_slot(const OptionalField<std::uint32_t>& slot) {
        m_slot = slot;
    }

    /*!
     * @brief Gets slot
     * @return Slot
     * */
    OptionalField<std::uint32_t> get_slot() const {
        return m_slot;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class MemoryLocation from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed MemoryLocation object
     */
    static MemoryLocation from_json(const json::Json& json);

private:
    OptionalField<std::uint32_t> m_socket{};
    OptionalField<std::uint32_t> m_controller{};
    OptionalField<std::uint32_t> m_channel{};
    OptionalField<std::uint32_t> m_slot{};
};

}
}
}
