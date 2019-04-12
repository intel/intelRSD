/*!
 * @brief Implementation of health state of the resource
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
 * @file health_telemetry_reader.cpp
 */

#include "telemetry/health_telemetry_reader.hpp"
#include "telemetry/metric_definition_builder.hpp"
#include "ipmi/sel.hpp"

#include <map>

namespace {

static agent_framework::model::MetricDefinition empty_metric_definition = telemetry::MetricDefinitionBuilder{""}.build();

}

namespace telemetry {


HealthTelemetryReader::EventList::~EventList() {}


HealthTelemetryReader::Warnings::~Warnings() {}


HealthTelemetryReader::Critical::~Critical() {}


TelemetryReader::ReaderId HealthTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

/*!
 * @brief Class to process SEL records (events)
 */
class SelContext : public TelemetryReader::Context {
public:
    /*!
     * @brief Simple container for found record
     */
    class FoundRecord final {
    public:
        FoundRecord(const ipmi::SelRecordGeneric* _record, const char* _descr) : record(_record), descr(_descr) { }

        const ipmi::SelRecordGeneric* record;
        const char* descr;
    };
    using FoundRecords = std::vector<FoundRecord>;


    SelContext(ipmi::IpmiController& ipmi): m_sel(ipmi) { }

    /*!
     * @brief Update asserted events list
     * @return true if any (sensor) event has been added/removed
     *              or any (assertion) event was previously shored up.
     */
    bool update() override;

    /*!
     * @brief Find an event from given list if (asserted) in the SEL
     * @param list to be checked
     * @return true if any event from list is found in the SEL
     */
    FoundRecords asserting_events(const HealthTelemetryReader::EventList& list,
                                  double shoreup_period, bool& shored_up, bool only_first);

private:
    ipmi::Sel m_sel;
};


bool SelContext::update() {
    return m_sel.process_records();
}

SelContext::FoundRecords SelContext::asserting_events(const HealthTelemetryReader::EventList& list,
                                                      double shoreup_period, bool& shored_up, bool only_first) {
    FoundRecords ret{};

    for (auto event_def : list) {
        ipmi::Sel::RecordVect records = m_sel.get_records([event_def](ipmi::SelRecord::Ptr record) -> bool {
            const ipmi::SelRecordGeneric* event = dynamic_cast<const ipmi::SelRecordGeneric*>(record.get());
            if ((event != nullptr) && event->is_of_type(event_def.first)) {
                return true;
            }
            return false;
        });
        /* in the list there are assertions and deassertions, SEL keeps them in order, so very last
         * is the most important. First found asserted record breaks the loop.
         */
        if (!records.empty()) {
            const ipmi::SelRecordGeneric* event = static_cast<const ipmi::SelRecordGeneric*>(records.crbegin()->get());

            /* look for events to be shored up, only if the last one is deassertion */
            if (!event->is_asserted()) {
                event = nullptr;

                /* previous event causes alert if there is assertion within the shore up period */
                ipmi::Sel::RecordVect::const_reverse_iterator it;
                for (it = records.crbegin() + 1; it != records.crend(); it++) {
                    event = static_cast<const ipmi::SelRecordGeneric*>(it->get());
                    if (event->get_timestamp() + shoreup_period < m_sel.get_sel_time()) {
                        /* Event was reported, not necessary to shore it up anymore. */
                        event = nullptr;
                        break;
                    }
                    if (event->is_asserted()) {
                        shored_up = true;
                        break;
                    }
                    event = nullptr;
                }
            }

            if (event != nullptr) {
                ret.push_back({event, event_def.second});
                if (only_first) {
                    return ret;
                }
            }
        }
    }
    return ret;
}




HealthTelemetryReader::HealthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                                             Warnings warning_events, Critical critical_events) :
        TelemetryReader(_resource_key, _metric_definition, assigned_reader_id(), FILL_HEALTH | FILL_METRIC),
        m_warning_events(warning_events), m_critical_events(critical_events) { fill_discreete_values(); }

HealthTelemetryReader::HealthTelemetryReader(ResourceInstance _resource_key,
                                             Warnings warning_events, Critical critical_events) :
    TelemetryReader(_resource_key, empty_metric_definition, assigned_reader_id(), FILL_HEALTH),
    m_warning_events(warning_events), m_critical_events(critical_events) { }

HealthTelemetryReader::HealthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                                             Critical critical_events, Warnings warning_events) :
    TelemetryReader(_resource_key, _metric_definition, assigned_reader_id(), FILL_HEALTH | FILL_METRIC),
    m_warning_events(warning_events), m_critical_events(critical_events) { fill_discreete_values(); }

HealthTelemetryReader::HealthTelemetryReader(ResourceInstance _resource_key,
                                             Critical critical_events, Warnings warning_events) :
    TelemetryReader(_resource_key, empty_metric_definition, assigned_reader_id(), FILL_HEALTH),
    m_warning_events(warning_events), m_critical_events(critical_events) { }


void HealthTelemetryReader::fill_discreete_values() {
    /* don't set metric definition allowed values if no metric definition */
    if (get_metric_definition().get_metric_jsonptr().empty()) {
        return;
    }

    std::set<std::string> discreete_values{};
    for (auto& event : m_warning_events) {
        if (event.second != nullptr) {
            discreete_values.insert(event.second);
        } else {
            discreete_values.insert(agent_framework::model::enums::Health(
                agent_framework::model::enums::Health::Warning).to_string());
        }
    }

    for (auto& event : m_critical_events) {
        if (event.second != nullptr) {
            discreete_values.insert(event.second);
        } else {
            discreete_values.insert(agent_framework::model::enums::Health(
                agent_framework::model::enums::Health::Critical).to_string());
        }
    }

    discreete_values.insert(agent_framework::model::enums::Health(
        agent_framework::model::enums::Health::OK).to_string());

    m_metric_definition.set_discrete_values(std::vector<std::string>{discreete_values.begin(), discreete_values.end()});
}


TelemetryReader::Context::Ptr HealthTelemetryReader::create_context(ipmi::IpmiController& ipmi,
                                                                      TelemetryReader::PtrVector&) {
    return std::make_shared<SelContext>(ipmi);
}


bool HealthTelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    if (!get_metric_definition().get_metric_jsonptr().empty()) {
        if ((!get_metric_definition().get_metric_type().has_value()) ||
            (get_metric_definition().get_metric_type().value() != agent_framework::model::enums::MetricType::Discrete)) {

            log_error("telemetry", "Metric definition " << get_metric_definition().get_metric_jsonptr()
                                                        << " is not discrete");
        }
        if ((!get_metric_definition().get_discrete_metric_type().has_value()) ||
            (get_metric_definition().get_discrete_metric_type().value() != agent_framework::model::enums::DiscreteMetricType::Multiple)) {

            log_error("telemetry", "Metric definition " << get_metric_definition().get_metric_jsonptr()
                                                        << " is not multiple discrete");
        }
    }
    return !(m_warning_events.empty() && m_critical_events.empty());
}


bool HealthTelemetryReader::read(TelemetryReader::Context::Ptr context, ipmi::IpmiController&) {
    SelContext* ctx = static_cast<SelContext*>(context.get());
    double shoreup_period = get_metric_definition().get_shoreup_period();

    agent_framework::model::enums::Health::base_enum health{};
    std::set<std::string> events{};
    bool shored_up = false;

    /* Check events */
    bool only_first = get_metric_definition().get_metric_jsonptr().empty();
    do {
        SelContext::FoundRecords critical = ctx->asserting_events(get_critical_events(),
            shoreup_period, shored_up, only_first);
        if (!critical.empty()) {
            log_error("telemetry", get_resource_key() << " event " << critical.front().record->to_string()
                                            << " => critical");
            health = agent_framework::model::enums::Health::Critical;
            if (!only_first) {
                for (const auto& record : critical) {
                    if (record.descr != nullptr) {
                        events.insert(record.descr);
                    }
                }
            }
            break;
        }

        SelContext::FoundRecords warnings = ctx->asserting_events(get_warning_events(),
            shoreup_period, shored_up, only_first);
        if (!warnings.empty()) {
            log_warning("telemetry", get_resource_key() << " event " << warnings.front().record->to_string()
                                              << " => warning");
            health = agent_framework::model::enums::Health::Warning;
            if (!only_first) {
                for (const auto& record : warnings) {
                    if (record.descr != nullptr) {
                        events.insert(record.descr);
                    }
                }
            }
            break;
        }

        health = agent_framework::model::enums::Health::OK;
    } while (false);

    if (shored_up) {
        set_to_be_read(shored_up);
    }

    /* if no events found, add current health to the list. Mostly "OK" */
    if (events.empty()) {
        events.insert(agent_framework::model::enums::Health(health).to_string());
    }

    json::Json value = json::Json();
    if ((!get_metric_definition().get_discrete_metric_type().has_value()) ||
        (get_metric_definition().get_discrete_metric_type().value() != agent_framework::model::enums::DiscreteMetricType::Multiple)) {

        value = *(events.cbegin());
    }
    else {
        value = events;
    }

    bool changed = false;
    if ((get_health() != health) || (get_value() != value)) {
        set_value(value);
        changed = true;
    }
    if (changed) {
        set_health(health);
    }
    return changed;
}

}
