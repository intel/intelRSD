/*!
 * @brief Declaration of PerformanceConfiguration attribute class.
 * This class covers both Static Speed Select and Prioritized Base Frequency
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file performance_configuration.hpp
 */

#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

class PerformanceConfiguration {
public:

    PerformanceConfiguration() = default;
    ~PerformanceConfiguration();

    /*! Enable copy */
    PerformanceConfiguration(const PerformanceConfiguration&) = default;
    PerformanceConfiguration& operator=(const PerformanceConfiguration&) = default;
    PerformanceConfiguration(PerformanceConfiguration&&) = default;
    PerformanceConfiguration& operator=(PerformanceConfiguration&&) = default;


    /*!
     * @brief Converts PerformanceConfiguration to JSON object
     * @return JSON representation of PerformanceConfiguration object
     */
    json::Json to_json() const;


    /*!
     * @brief Constructs an object of class PerformanceConfiguration from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed PerformanceConfiguration object
     */
    static PerformanceConfiguration from_json(const json::Json& json);


    /*!
     * @brief Get configuration id
     * @return configuration id
     */
    const OptionalField<uint64_t>& get_configuration_id() const {
        return m_configuration_id;
    }


    /*!
     * @brief Set configuration id
     * @param[in] configuration_id the configuration id
     */
    void set_configuration_id(const OptionalField<uint64_t>& configuration_id) {
        m_configuration_id = configuration_id;
    }


    /*!
     * @brief Get configuration type
     * @return configuration type
     */
    const OptionalField<enums::PerformanceConfigurationType>& get_type() const {
        return m_type;
    }


    /*!
     * @brief Set configuration type
     * @param[in] type the configuration type
     */
    void set_type(const OptionalField<enums::PerformanceConfigurationType>& type) {
        m_type = type;
    }


    /*!
     * @brief Get high priority core count
     * @return high priority core count
     */
    const OptionalField<uint64_t>& get_high_priority_core_count() const {
        return m_high_priority_core_count;
    }


    /*!
     * @brief Set high priority core count
     * @param[in] high_priority_core_count the high priority core count
     */
    void set_high_priority_core_count(const OptionalField<uint64_t>& high_priority_core_count) {
        m_high_priority_core_count = high_priority_core_count;
    }


    /*!
     * @brief Get low priority core count
     * @return low priority core count
     */
    const OptionalField<uint64_t>& get_low_priority_core_count() const {
        return m_low_priority_core_count;
    }


    /*!
     * @brief Set low priority core count
     * @param[in] low_priority_core_count the low priority core count
     */
    void set_low_priority_core_count(const OptionalField<uint64_t>& low_priority_core_count) {
        m_low_priority_core_count = low_priority_core_count;
    }


    /*!
     * @brief Get high priority base frequency
     * @return high priority base frequency
     */
    const OptionalField<uint64_t>& get_high_priority_base_frequency() const {
        return m_high_priority_base_frequency;
    }


    /*!
     * @brief Set high priority base frequency
     * @param[in] high_priority_base_frequency the high priority base frequency
     */
    void set_high_priority_base_frequency(const OptionalField<uint64_t>& high_priority_base_frequency) {
        m_high_priority_base_frequency = high_priority_base_frequency;
    }


    /*!
     * @brief Get low priority base frequency
     * @return low priority base frequency
     */
    const OptionalField<uint64_t>& get_low_priority_base_frequency() const {
        return m_low_priority_base_frequency;
    }


    /*!
     * @brief Set low priority base frequency
     * @param[in] low_priority_base_frequency the low priority base frequency
     */
    void set_low_priority_base_frequency(const OptionalField<uint64_t>& low_priority_base_frequency) {
        m_low_priority_base_frequency = low_priority_base_frequency;
    }


    /*!
     * @brief Get active cores
     * @return active cores
     */
    const OptionalField<uint64_t>& get_active_cores() const {
        return m_active_cores;
    }


    /*!
     * @brief Set active cores
     * @param[in] active_cores the active cores
     */
    void set_active_cores(const OptionalField<uint64_t>& active_cores) {
        m_active_cores = active_cores;
    }


    /*!
     * @brief Get base core frequency
     * @return base core frequency
     */
    const OptionalField<uint64_t>& get_base_core_frequency() const {
        return m_base_core_frequency;
    }


    /*!
     * @brief Set base core frequency
     * @param[in] base_core_frequency the base core frequency
     */
    void set_base_core_frequency(const OptionalField<uint64_t>& base_core_frequency) {
        m_base_core_frequency = base_core_frequency;
    }


    /*!
     * @brief Get max TDP per processor
     * @return max TDP per processor
     */
    const OptionalField<uint64_t>& get_tdp() const {
        return m_tdp;
    }


    /*!
     * @brief Set max TDP per processor
     * @param[in] tdp the max TDP per processor
     */
    void set_tdp(const OptionalField<uint64_t>& tdp) {
        m_tdp = tdp;
    }


    /*!
     * @brief Get max processor junction temperature in Celsius
     * @return max processor junction temperature in Celsius
     */
    const OptionalField<uint64_t>& get_max_junction_temp_celsius() const {
        return m_max_junction_temp_celsius;
    }


    /*!
     * @brief Set max processor junction temperature in Celsius
     * @param[in] max_junction_temp_celsius the max processor junction temperature in Celsius
     */
    void set_max_junction_temp_celsius(const OptionalField<uint64_t>& max_junction_temp_celsius) {
        m_max_junction_temp_celsius = max_junction_temp_celsius;
    }


private:
    /* common configuration members */
    OptionalField<uint64_t> m_configuration_id{};
    OptionalField<enums::PerformanceConfigurationType> m_type{};
    OptionalField<uint64_t> m_tdp{};
    OptionalField<uint64_t> m_max_junction_temp_celsius{};

    /* prioritized base frequency */
    OptionalField<uint64_t> m_high_priority_core_count{};
    OptionalField<uint64_t> m_low_priority_core_count{};
    OptionalField<uint64_t> m_high_priority_base_frequency{};
    OptionalField<uint64_t> m_low_priority_base_frequency{};

    /* speed select */
    OptionalField<uint64_t> m_active_cores{};
    OptionalField<uint64_t> m_base_core_frequency{};
};

}
}
}