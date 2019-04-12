/*!
 * @brief Processor for all metrics
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
 * @file metric_processor.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"

#include <map>

namespace telemetry {

class MetricsProcessor final {
public:
    /*!
     * @brief Build metrics processor for all metrics on single sled.
     *
     * All readers share metric definitions. Metrics processor is assigned to the platform,
     * each processor handles only one sled.
     *
     * @param _ctrl controller to communicate with sled
     * @param _reader_ptrs metrics to be read from sled
     */
    MetricsProcessor(ipmi::IpmiController& _ctrl, TelemetryReader::PtrVector&& _reader_ptrs) :
        ctrl(_ctrl), reader_ptrs(std::move(_reader_ptrs)) { }


    /*!
     * @brief Getter for all defined readers handled by the processor
     * @return vector of all defined readers
     */
    const TelemetryReader::PtrVector& get_reader_ptrs() const { return reader_ptrs; }

    /*!
     * @brief Read all (properly configured) metrics
     * @return true if any metric was changed
     */
    TelemetryReader::PtrVector read_all_metrics();

    /*!
     * @brief Get first reader time to be executed
     * @return closest time when readers should be executed
     */
    TelemetryReader::TimePoint get_earliest_update_time() const;

private:
    ipmi::IpmiController& ctrl;
    TelemetryReader::PtrVector reader_ptrs;

    /* configs for used readers */
    using Contexts = std::map<TelemetryReader::ReaderId, TelemetryReader::Context::Ptr>;
    Contexts contexts{};

    /* reader name and how "ready" is: to be used in read_all_metrics() */
    using ReaderReady = std::pair<std::string, unsigned>;
    using ReadersReadyMap = std::map<TelemetryReader::ReaderId, ReaderReady>;
    ReadersReadyMap ready_readers{};

    static bool is_time_passed(const TelemetryReader& reader, TelemetryReader::TimePoint now);
    static bool has_proper_period(const TelemetryReader& reader);

    std::set<TelemetryReader::Ptr> remove_not_valid_readers(TelemetryReader::PtrVector& readers,
                                                             const TelemetryReader::Context::Ptr& context);

    void update_reader_timestamp(TelemetryReader& reader, TelemetryReader::TimePoint now);
};

}
