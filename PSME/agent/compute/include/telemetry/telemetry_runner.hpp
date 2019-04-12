/*!
 * @brief TelemetryRunner.
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
 * @file compute/include/status/telemetry_runner.hpp
 */

#pragma once

#include <chrono>

namespace agent {
namespace compute {
// forward declaration
class Bmc;
}
}

namespace telemetry {

class TelemetryRunner {
public:
    /*!
     * @brief Single telemetry processing run
     * @param bmc Object maintaining connection to the sled
     * @param interval to check if data is to be processed
     * @return Next time when telemetry should be processed
     */
    std::chrono::steady_clock::time_point::duration run(
            agent::compute::Bmc& bmc, std::chrono::steady_clock::time_point::duration interval);

private:
    unsigned m_runs_counter{};
    unsigned m_changes_counter{};
};

}
