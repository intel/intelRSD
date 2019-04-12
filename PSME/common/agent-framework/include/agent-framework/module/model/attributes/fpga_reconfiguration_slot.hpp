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
 *
 * @file fpga_reconfiguration_slot.hpp
 * @brief FPGA reconfiguration slot attribute
 * */



#pragma once



#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

class FpgaReconfigurationSlot {
public:
    /*!
     * @brief Set FPGA slot ID.
     *
     * @param slot_id FPGA slot ID.
     */
    void set_slot_id(const OptionalField<std::string>& slot_id) {
        m_slot_id = slot_id;
    }


    /*!
     * @brief Get FPGA slot ID.
     *
     * @return FPGA slot ID.
     */
    const OptionalField<std::string>& get_slot_id() const {
        return m_slot_id;
    }


    /*!
     * @brief Set FPGA slot UUID.
     *
     * @param uuid FPGA slot UUID.
     */
    void set_uuid(const OptionalField<std::string>& uuid) {
        m_uuid = uuid;
    }


    /*!
     * @brief Get FPGA slot UUID.
     *
     * @return FPGA slot UUID.
     */
    const OptionalField<std::string> get_uuid() const {
        return m_uuid;
    }


    /*!
     * @brief Set FPGA slot programmable from host.
     *
     * @param programmable_from_host FPGA slot programmable from host.
     */
    void set_programmable_from_host(const OptionalField<bool>& programmable_from_host) {
        m_programmable_from_host = programmable_from_host;
    }


    /*!
     * @brief Get FPGA slot programmable from host.
     *
     * @return FPGA slot programmable from host.
     */
    const OptionalField<bool> get_programmable_from_host() const {
        return m_programmable_from_host;
    }


    /*!
     * @brief construct an object of class FpgaReconfigurationSlot from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed FpgaReconfigurationSlot object
     */
    static FpgaReconfigurationSlot from_json(const json::Json& json);


    /*!
     * @brief Transform the object to Json
     *
     * @return An FpgaReconfigurationSlot object serialized to json::Json
     */
    json::Json to_json() const;


private:
    OptionalField<std::string> m_slot_id{};
    OptionalField<std::string> m_uuid{};
    OptionalField<bool> m_programmable_from_host{};
};

}
}
}
