/*!
 * @brief Memory: Power Management Policy attribute
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file power_management_policy.hpp
 */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"


namespace agent_framework {
namespace model {
namespace attribute {

/*! Memory: Power Management Policy */
class PowerManagementPolicy {
public:

    explicit PowerManagementPolicy();

    PowerManagementPolicy(const bool policy_enabled,
                          const std::uint32_t max_tdp_milliwatts,
                          const std::uint32_t average_power_budget_milliwatts,
                          const std::uint32_t peak_power_budget_milliwatts):
        m_policy_enabled{policy_enabled},
        m_max_tdp_milliwatts{max_tdp_milliwatts},
        m_average_power_budget_milliwatts{average_power_budget_milliwatts},
        m_peak_power_budget_milliwatts{peak_power_budget_milliwatts}
    {}

    ~PowerManagementPolicy();

    /*! Enable copy */
    PowerManagementPolicy(const PowerManagementPolicy&) = default;
    PowerManagementPolicy& operator=(const PowerManagementPolicy&) = default;
    PowerManagementPolicy(PowerManagementPolicy&&) = default;
    PowerManagementPolicy& operator=(PowerManagementPolicy&&) = default;

    /*!
     * @brief Sets policy enabled
     * @param[in] policy_enabled Policy enabled
     * */
    void set_policy_enabled(const OptionalField<bool>& policy_enabled) {
        m_policy_enabled = policy_enabled;
    }

    /*!
     * @brief Gets policy enabled
     * @return Policy enabled
     * */
    OptionalField<bool> get_policy_enabled() const {
        return m_policy_enabled;
    }

    /*!
     * @brief Sets max TDP in milli watts
     * @param[in] max_tdp_milliwatts Max TDP (in mW)
     * */
    void set_max_tdp_milliwatts(const OptionalField<std::uint32_t>& max_tdp_milliwatts) {
        m_max_tdp_milliwatts = max_tdp_milliwatts;
    }

    /*!
     * @brief Gets max TDP in milli watts
     * @return Max TDP (in mW)
     * */
    OptionalField<std::uint32_t> get_max_tdp_milliwatts() const {
        return m_max_tdp_milliwatts;
    }

    /*!
     * @brief Sets average power budget in milli watts
     * @param[in] average_power_budget_milliwatts Average power budget (in mW)
     * */

    void set_average_power_budget_milliwatts(const OptionalField<std::uint32_t>& average_power_budget_milliwatts) {
        m_average_power_budget_milliwatts = average_power_budget_milliwatts;
    }

    /*!
     * @brief Gets average power budget in milli watts
     * @return Average power budget (in mW)
     * */
    const OptionalField<std::uint32_t>& get_average_power_budget_milliwatts() const {
        return m_average_power_budget_milliwatts;
    }

    /*!
     * @brief Sets peak power budget in milli watts
     * @param[in] peak_power_budget_milliwatts Peak power budget (in mW)
     * */
    void set_peak_power_budget_milliwatts(const OptionalField<std::uint32_t>& peak_power_budget_milliwatts) {
        m_peak_power_budget_milliwatts = peak_power_budget_milliwatts;
    }

    /*!
     * @brief Gets peak power budget in milli watts
     * @return Peak power budget (in mW)
     * */
    const OptionalField<std::uint32_t>& get_peak_power_budget_milliwatts() const {
        return m_peak_power_budget_milliwatts;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class PowerManagementPolicy from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed PowerManagementPolicy object
     */
    static PowerManagementPolicy from_json(const json::Json& json);

private:
    OptionalField<bool> m_policy_enabled{};
    OptionalField<std::uint32_t> m_max_tdp_milliwatts{};
    OptionalField<std::uint32_t> m_average_power_budget_milliwatts{};
    OptionalField<std::uint32_t> m_peak_power_budget_milliwatts{};
};

}
}
}