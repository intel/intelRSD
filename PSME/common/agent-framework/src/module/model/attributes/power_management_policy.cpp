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
 * @file power_management_policy.cpp
 */

#include "agent-framework/module/model/attributes/power_management_policy.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

PowerManagementPolicy::PowerManagementPolicy() { }

PowerManagementPolicy::~PowerManagementPolicy() { }

json::Json PowerManagementPolicy::to_json() const {
    json::Json result = json::Json();
    result[literals::PowerManagementPolicy::POLICY_ENABLED] = get_policy_enabled();
    result[literals::PowerManagementPolicy::MAX_TDP_MILLIWATTS] = get_max_tdp_milliwatts();
    result[literals::PowerManagementPolicy::AVERAGE_POWER_BUDGET_MILLIWATTS] = get_average_power_budget_milliwatts();
    result[literals::PowerManagementPolicy::PEAK_POWER_BUDGET_MILLIWATTS] = get_peak_power_budget_milliwatts();
    return result;
}

PowerManagementPolicy PowerManagementPolicy::from_json(const json::Json& json) {
    attribute::PowerManagementPolicy region{};
    region.set_policy_enabled(json[literals::PowerManagementPolicy::POLICY_ENABLED]);
    region.set_max_tdp_milliwatts(json[literals::PowerManagementPolicy::MAX_TDP_MILLIWATTS]);
    region.set_average_power_budget_milliwatts(json[literals::PowerManagementPolicy::AVERAGE_POWER_BUDGET_MILLIWATTS]);
    region.set_peak_power_budget_milliwatts(json[literals::PowerManagementPolicy::PEAK_POWER_BUDGET_MILLIWATTS]);
    return region;
}