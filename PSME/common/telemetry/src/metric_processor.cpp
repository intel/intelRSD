/*!
 * @brief Processor for all metrics
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file metric_processor.cpp
 */

#include "telemetry/metric_processor.hpp"
#include "logger/logger_factory.hpp"

#include <set>

namespace telemetry {

bool MetricsProcessor::is_time_passed(const TelemetryReader& reader, TelemetryReader::SampleTime now) {
    return (!reader.time_to_update_set)
        || (std::chrono::duration_cast<std::chrono::milliseconds>(
               reader.next_time_to_update - now) <= std::chrono::milliseconds::zero());
}

bool MetricsProcessor::has_proper_period(const TelemetryReader& reader) {
    return (reader.get_reader_state() != TelemetryReader::State::NOT_VALID)
           && std::chrono::duration_cast<std::chrono::milliseconds>(
        reader.get_metric_definition().get_sensing_period()) > std::chrono::milliseconds::zero();
}

TelemetryReader::PtrVector MetricsProcessor::read_all_metrics() {
    TelemetryReader::SampleTime now = std::chrono::steady_clock::now();

    TelemetryReader::PtrVector modified{};

    /* group all defined readers by type */
    using DistributedReadersMap = std::map<TelemetryReader::ReaderId, TelemetryReader::PtrVector>;
    DistributedReadersMap distributed_readers{};
    for (auto& reader : reader_ptrs) {
        distributed_readers[reader->get_reader_id()].push_back(reader);
    }

    /* create all missing contexts */
    for (auto& group : distributed_readers) {
        TelemetryReader::ReaderId id = group.first;
        TelemetryReader::PtrVector& readers = group.second;

        /* process context if any reader's time has passed */
        bool found = false;
        for (auto& reader : readers) {
            if (is_time_passed(*reader, now)) {
                found = true;
            }
        }
        if (!found) {
            continue;
        }

        /* get context, or create new one if not created before */
        TelemetryReader::Context::Ptr context{};
        if (contexts.count(id) != 0) {
            context = contexts[id];
        }
        else {
            try {
                context = readers.front()->create_context(ctrl, reader_ptrs);
                contexts[id] = context;
            }
            catch (std::runtime_error& e) {
                log_error("telemetry", "Runtime error during context creation for "
                    << readers.front()->get_info() << ":: " << e.what());
                /* check next group */
                continue;
            }
            /* check all readers if not duplicated and set reader validity */
            for (auto& reader : readers) {
                bool valid = false;
                try {
                    valid = reader->is_valid(context);
                }
                catch (std::runtime_error& e) {
                    log_error("telemetry", "Runtime error during validating reader, "
                        << "reader " << reader->get_info() << ":: " << e.what());
                }
                if (valid) {
                    reader->set_reader_state(TelemetryReader::State::VALUE_NOT_PRESENT);
                } else {
                    log_warning("telemetry", reader->get_info() << reader->get_resource_key() << " is not valid");
                }
            }
        }

        /* Update context data, if any reader is to be updated. Metrics with zero interval are not updated. */
        bool update_context = false;
        for (const auto& reader : readers) {
            if (is_time_passed(*reader, now) && has_proper_period(*reader)) {
                update_context = true;
                break;
            }
        }
        if (update_context) {
            try {
                if ((context == nullptr) || context->update()) {
                    for (auto& reader : readers) {
                        /* regardless time has passed */
                        if (has_proper_period(*reader)) {
                            reader->to_be_read = true;
                        }
                    }
                }
            }
            catch (std::runtime_error& e) {
                log_error("telemetry", "Runtime error during update context, "
                    << "reader " << readers.front()->get_info() << ":: " << e.what());
                /* clear all readers, context is not up-to-date! */
                for (auto& reader : readers) {
                    if (reader->clear_value()) {
                        modified.push_back(reader);
                    }
                }
            }
        }

        /* update timestamp for all valid readers and the data for those with changed context */
        for (auto& reader : readers) {
            bool time_passed = false;
            if (is_time_passed(*reader, now)) {
                /* corner case: sensing period is zero for valid reader */
                if (has_proper_period(*reader)) {
                    if (!reader->time_to_update_set) {
                        reader->next_time_to_update = now;
                        reader->time_to_update_set = true;
                    }
                    /* some runs might be missed, keep periodicity */
                    do {
                        reader->next_time_to_update += std::chrono::duration_cast<std::chrono::milliseconds>(
                            reader->get_metric_definition().get_sensing_period());
                    }
                    while (is_time_passed(*reader, now));
                    time_passed = true;
                }
                else if (reader->time_to_update_set) {
                    log_error("telemetry", "Reader " << reader->get_info() << " has zero sensing interval set!");
                    reader->time_to_update_set = false;
                }
            }

            if (reader->to_be_read && time_passed) {
                reader->to_be_read = false;

                try {
                    /* value is updated only when reader is valid */
                    if (reader->read(context, ctrl)) {
                        modified.push_back(reader);
                    }
                }
                catch (std::runtime_error& e) {
                    log_error("telemetry", "Runtime error during reading value, "
                        << "reader " << reader->get_info() << reader->get_resource_key() << ":: " << e.what());
                    if (reader->clear_value()) {
                        modified.push_back(reader);
                    }
                }
            }
        }
    }

    /* all reader classes has been processed */
    return modified;
}

std::chrono::time_point<std::chrono::steady_clock> MetricsProcessor::get_earliest_update_time() const {
    std::chrono::time_point<std::chrono::steady_clock> time = std::chrono::steady_clock::now();
    bool first = true;
    for (const auto& reader : reader_ptrs) {
        if (reader->time_to_update_set) {
            if (first) {
                time = reader->get_time_to_update();
                first = false;
            }
            else if (reader->get_time_to_update() < time) {
                time = reader->get_time_to_update();
            }
        }
    }
    return time;
}


}
