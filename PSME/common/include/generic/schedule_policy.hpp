/*!
 * @brief SchedulePolicy.
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
 * @file generic/schedule_policy.hpp
 */

#pragma once

namespace generic {

/*! SchedulePolicyType enumeration */
enum class SchedulePolicyType {
    FIXED_RATE,
    FIXED_DELAY
};

template<typename Clock, SchedulePolicyType>
struct SchedulePolicy {};

template<typename Clock>
struct SchedulePolicy<Clock, SchedulePolicyType::FIXED_DELAY> {
    using TimePoint = typename Clock::time_point;
    using Duration = typename Clock::duration;

    /*!
     * @brief SchedulePolicyType getter
     * @return SchedulePolicyType
     */
    static constexpr SchedulePolicyType get_schedule_policy_type() noexcept {
        return SchedulePolicyType::FIXED_DELAY;
    }

    /*!
     * @brief Calculates next timepoint based on duration and last timepoint
     * @param duration Duration
     * @return Next timepoint
     */
    static TimePoint next_timepoint(Duration duration, TimePoint) {
        return (duration > Duration::zero())
               ? Clock::now() + duration
               : Clock::now() + std::chrono::milliseconds(1);
    }
};

template<typename Clock>
struct SchedulePolicy<Clock, SchedulePolicyType::FIXED_RATE> {
    using TimePoint = typename Clock::time_point;
    using Duration = typename Clock::duration;

    /*!
     * @brief SchedulePolicyType getter
     * @return SchedulePolicyType
     */
    static constexpr SchedulePolicyType get_schedule_policy_type() noexcept {
        return SchedulePolicyType::FIXED_RATE;
    }

    /*!
     * @brief Calculates next timepoint based on duration and last timepoint
     * @param duration Duration
     * @param timepoint Last scheduled timepoint
     * @return Next timepoint
     */
    static TimePoint next_timepoint(Duration duration, TimePoint timepoint) {
        const auto now = Clock::now();
        if (duration > Duration::zero()) {
            /* keep task running in a synchronic way */
            while (timepoint < now) {
                timepoint += duration;
            }
        } else {
            /* keep task running in a non-synchronic way, but execute all next "past" tasks */
            timepoint = now + std::chrono::milliseconds(1);
        }
        return timepoint;
    }
};

template<typename Clock>
using FixedDelaySchedulePolicy = SchedulePolicy<Clock, SchedulePolicyType::FIXED_DELAY>;

template<typename Clock>
using FixedRateSchedulePolicy = SchedulePolicy<Clock, SchedulePolicyType::FIXED_RATE>;

}
