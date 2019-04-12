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
 * @file metric_processor.cpp
 */

#include "telemetry/metric_processor.hpp"
#include "logger/logger_factory.hpp"

#include <set>

namespace telemetry {

bool MetricsProcessor::is_time_passed(const TelemetryReader& reader, TelemetryReader::TimePoint now) {
    return !reader.m_next_time_to_update.has_value()
           || (reader.m_next_time_to_update.value() - now) <= TelemetryReader::TimePoint::duration::zero();
}

void MetricsProcessor::update_reader_timestamp(TelemetryReader& reader, TelemetryReader::TimePoint now) {
    if (!reader.m_next_time_to_update.has_value()) {
        reader.m_next_time_to_update = now;
    }
    /* some runs might be missed, keep periodicity */
    do {
        reader.m_next_time_to_update.value() += reader.get_metric_definition().get_sensing_period().get_milliseconds();
    } while (is_time_passed(reader, now));
}

bool MetricsProcessor::has_proper_period(const TelemetryReader& reader) {
    return reader.get_metric_definition().get_sensing_period() > agent_framework::utils::Iso8601TimeInterval::zero();
}

std::set<TelemetryReader::Ptr> MetricsProcessor::remove_not_valid_readers(TelemetryReader::PtrVector& readers,
                                                                          const TelemetryReader::Context::Ptr& context) {
    auto it = std::remove_if(std::begin(readers), std::end(readers),
        [context](const TelemetryReader::Ptr & reader) {
            try {
                if (!reader->is_valid(context)) {
                    log_warning("telemetry", reader->get_info() << reader->get_resource_key() << " is not valid");
                    return true;
                }
                return false;
            } catch (const std::runtime_error& e) {
                log_error("telemetry", "Runtime error during validating reader, "
                        << "reader " << reader->get_info() << ":: " << e.what());
            }
            return true;
        });
    std::set<TelemetryReader::Ptr> not_valid_readers(it, std::end(readers));
    readers.erase(it, std::end(readers));
    // remove not valid readers
    reader_ptrs.erase(std::remove_if(std::begin(reader_ptrs), std::end(reader_ptrs),
                                     [not_valid_readers](const TelemetryReader::Ptr& reader) {
                                        return not_valid_readers.find(reader) != not_valid_readers.end();
                                     }), std::end(reader_ptrs));
    return not_valid_readers;
}

TelemetryReader::PtrVector MetricsProcessor::read_all_metrics() {
    auto now = TelemetryReader::TimePoint::clock::now();

    TelemetryReader::PtrVector modified{};

    /* group all defined readers by type */
    using DistributedReadersMap = std::map<TelemetryReader::ReaderId, TelemetryReader::PtrVector>;
    DistributedReadersMap distributed_readers{};
    for (auto& reader : reader_ptrs) {
        /* do not process readers with invalid sensing interval */
        if (has_proper_period(*reader)) {
            distributed_readers[reader->get_reader_id()].push_back(reader);
        }
    }

    /* create all missing contexts */
    for (auto& group : distributed_readers) {
        TelemetryReader::ReaderId id = group.first;
        TelemetryReader::PtrVector& readers = group.second;

        /* process context if any reader's time has passed */
        if (std::none_of(std::begin(readers), std::end(readers),
                [now](const TelemetryReader::Ptr& reader) { return is_time_passed(*reader, now);})) {
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
            /* remove wrongly configured readers */
            remove_not_valid_readers(readers, context);
        }

        /* Update context data, if any reader is to be updated. Metrics with zero interval are not updated. */
        try {
            if ((context == nullptr) || context->update()) {
                for (auto& reader : readers) {
                    /* regardless time has passed */
                    reader->m_to_be_read = true;
                }
            }
        }
        catch (std::runtime_error& e) {
            log_error("telemetry", "Runtime error during update context, "
                << "reader " << readers.front()->get_info() << ":: " << e.what());
            /* clear all readers, context is not up-to-date! */
            for (auto& reader : readers) {
                if (reader->update_value(nullptr)) {
                    modified.push_back(reader);
                }
            }
        }

        /* update timestamp for all valid readers and the data for those with changed context */
        for (auto& reader : readers) {
            if (is_time_passed(*reader, now)) {
                update_reader_timestamp(*reader, now);
                if (reader->m_to_be_read) {
                    reader->m_to_be_read = false;

                    try {
                        if (reader->read(context, ctrl)) {
                            modified.push_back(reader);
                        }
                    } catch (std::runtime_error& e) {
                        log_error("telemetry", "Runtime error during reading value, "
                                << "reader " << reader->get_info() << reader->get_resource_key() << ":: " << e.what());
                        if (reader->update_value(nullptr)) {
                            modified.push_back(reader);
                        }
                    }
                }
            }
        }
    }

    /* all reader classes has been processed */
    return modified;
}

TelemetryReader::TimePoint MetricsProcessor::get_earliest_update_time() const {
    // comparator for finding minimal time (unset time is greater than any set time)
    const auto reader_by_time_compare = [](const TelemetryReader::Ptr& a, const TelemetryReader::Ptr& b) {
      if (a->m_next_time_to_update) {
        return b->m_next_time_to_update ? *(a->m_next_time_to_update) < *(b->m_next_time_to_update) : true;
      }
      return false;
    };
    const auto it = std::min_element(reader_ptrs.cbegin(), reader_ptrs.cend(), reader_by_time_compare);
    if (it != reader_ptrs.cend() && (*it)->m_next_time_to_update) {
        return *(*it)->m_next_time_to_update;
    }
    // no reader has time to update set
    return TelemetryReader::TimePoint::clock::now();
}


}
