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
 * @brief Declaration of PSU resource class
 * @file psu.hpp
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

class Psu : public Resource {
public:

    explicit Psu(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~Psu();


    Psu(const Psu&) = default;


    Psu& operator=(const Psu&) = default;


    Psu(Psu&&) = default;


    Psu& operator=(Psu&&) = default;


    /*!
     * @brief construct an object of class Psu from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Psu object
     */
    static Psu from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    static enums::CollectionName get_collection_name() {
        return Psu::collection_name;
    }


    static enums::Component get_component() {
        return Psu::component;
    }


    /*!
     * @brief Returns power supply type
     * @return Current value
     */
    const OptionalField<enums::PowerSupplyType>& get_power_supply_type() const {
        return m_power_supply_type;
    }


    /*!
     * @brief Sets power supply type
     * @param[in] power_supply_type New value
     */
    void set_power_supply_type(const OptionalField<enums::PowerSupplyType>& power_supply_type) {
        m_power_supply_type = power_supply_type;
    }


    /*!
     * @brief Returns line input voltage type
     * @return Current value
     */
    const OptionalField<enums::LineInputVoltageType>& get_line_input_voltage_type() const {
        return m_line_input_voltage_type;
    }


    /*!
     * @brief Sets line input voltage type
     * @param[in] line_input_voltage_type New value
     */
    void set_line_input_voltage_type(const OptionalField<enums::LineInputVoltageType>& line_input_voltage_type) {
        m_line_input_voltage_type = line_input_voltage_type;
    }


    /*!
     * @brief Returns line input voltage [V]
     * @return Current value
     */
    const OptionalField<double>& get_line_input_voltage_volts() const {
        return m_line_input_voltage_volts;
    }


    /*!
     * @brief Sets line input voltage [V]
     * @param[in] line_input_voltage_volts New value
     */
    void set_line_input_voltage_volts(const OptionalField<double>& line_input_voltage_volts) {
        m_line_input_voltage_volts = line_input_voltage_volts;
    }


    /*!
     * @brief Returns firmware version
     * @return Current value
     */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }


    /*!
     * @brief Sets firmware version
     * @param[in] firmware_version New value
     */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }


    /*!
     * @brief Returns power_capacity_watts
     * @return Current value
     */
    const OptionalField<double>& get_power_capacity_watts() const {
        return m_power_capacity_watts;
    }


    /*!
     * @brief Sets power_capacity_watts
     * @param[in] power_capacity_watts New value
     */
    void set_power_capacity_watts(const OptionalField<double>& power_capacity_watts) {
        m_power_capacity_watts = power_capacity_watts;
    }


    /*!
     * @brief Returns last_power_output_watts
     * @return Current value
     */
    const OptionalField<double>& get_last_power_output_watts() const {
        return m_last_power_output_watts;
    }


    /*!
     * @brief Sets last_power_output_watts
     * @param[in] last_power_output_watts New value
     */
    void set_last_power_output_watts(const OptionalField<double>& last_power_output_watts) {
        m_last_power_output_watts = last_power_output_watts;
    }


    /*!
     * @brief return an object fru_info of class attribute::FruInfo
     *
     * @return the fru_info value
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }


    /*!
     * @brief setter for fru_info attribute
     *
     * @param fru_info of type attribute::FruInfo
     */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Gets slot id value
     * @return Slot id value
     */
    uint8_t get_slot_id() const {
        return m_slot_id;
    }

    /*!
     * @brief Sets slot id value
     * @param slot_id new slot id value
     */
    void set_slot_id(const uint8_t slot_id) {
        m_slot_id = slot_id;
    }

    /*!
     * @brief Return an indicator LED value of enum enums::IndicatorLed
     * @return The indicator LED value
     * */
    const OptionalField<enums::IndicatorLed>& get_indicator_led() const {
        return m_indicator_led;
    }


    /*!
     * @brief Setter for indicator LED
     * @param indicator_led Indicator LED of type enums::IndicatorLed
     */
    void set_indicator_led(const OptionalField<enums::IndicatorLed>& indicator_led) {
        m_indicator_led = indicator_led;
    }


private:

    OptionalField<enums::PowerSupplyType> m_power_supply_type{};
    OptionalField<enums::LineInputVoltageType> m_line_input_voltage_type{};
    OptionalField<double> m_line_input_voltage_volts{};
    OptionalField<double> m_power_capacity_watts{};
    OptionalField<double> m_last_power_output_watts{};
    OptionalField<std::string> m_firmware_version{};
    OptionalField<enums::IndicatorLed> m_indicator_led{};
    attribute::FruInfo m_fru_info{};

    // agent only data
    uint8_t m_slot_id{};    // physical slot id

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
