/*!
 * @brief Telemetry service
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
 * @file telemetry_service.cpp
 */

#include "telemetry/telemetry_service.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "logger/logger_factory.hpp"

#include "configuration/configuration.hpp"
#include "configuration/configuration_validator.hpp"
#include "agent-framework/module/utils/iso8601_time_interval.hpp"

#include <limits>



namespace {

using telemetry::MetricsProcessor;
using telemetry::TelemetryReader;
using agent_framework::model::MetricDefinition;
using agent_framework::model::Metric;
using agent_framework::model::enums::Component;
using agent_framework::utils::Iso8601TimeInterval;

using namespace agent_framework::module;


std::string read_interval(const json::Json& telemetry_config,
                          const std::string& interval_property_name,
                          const std::string& default_value = "PT10S") {
    if (!telemetry_config.is_null() && telemetry_config.count(interval_property_name)) {
        const auto& json_prop = telemetry_config[interval_property_name];
        try {
            Iso8601TimeInterval interval{};
            if (json_prop.is_string()) {
                interval = Iso8601TimeInterval::parse(json_prop.get<std::string>());
            }
            else if (json_prop.is_number()) {
                using namespace std::chrono;
                interval = Iso8601TimeInterval(duration_cast<milliseconds>(duration<double>(json_prop.get<double>())));
            }
            if (interval > Iso8601TimeInterval::zero()) {
                return interval.to_string();
            }
            else {
                log_warning("telemetry", "Incorrect zero interval '" << interval_property_name
                                                                     << "', using default value: " << default_value);
            }
        }
        catch (const Iso8601TimeInterval::Exception&) {
            log_warning("telemetry", "Incorrect interval '" << interval_property_name
                                                            << "', using default value: " << default_value);
        }
    }
    return default_value;
}


/*!
 * @brief Process health from the reader
 *
 * @tparam R type of the component
 *
 * @param reader to get value from
 * @param events collection of events to append health-changed event
 * @return true if reader is for requested resource
 */
template<typename R>
bool update_health(const TelemetryReader& reader, agent_framework::model::attribute::EventData::Vector& events) {
    if (R::get_component() == reader.get_resource_key().get_component()) {
        if (get_manager<R>().entry_exists(reader.get_resource_uuid())) {
            auto entry = get_manager<R>().get_entry_reference(reader.get_resource_uuid());
            auto status = entry->get_status();

            OptionalField<agent_framework::model::enums::Health> health{};
            if (status.get_state() == agent_framework::model::enums::State::Absent) {
                log_info("telemetry", "Resource " << reader.get_resource_key() << " is absent");
            }
            else {
                health = reader.get_health();
                log_info("telemetry", "Health for " << reader.get_resource_key()
                                                    << " uuid '" << reader.get_resource_uuid() << "'"
                                                    << " set to " << health->to_string() << "/" << reader.get_value());
            }

            /* if health is not changed, don't send an event! */
            if (status.get_health() == health) {
                return true;
            }
            status.set_health(health);
            entry->set_status(status);
            // collect event
            agent_framework::model::attribute::EventData event{};
            event.set_parent(entry->get_parent_uuid());
            event.set_component(entry->get_uuid());
            event.set_type(entry->get_component());
            event.set_notification(agent_framework::model::enums::Notification::Update);
            events.push_back(std::move(event));
            return true;
        }
    }
    return false;
}

}

namespace telemetry {

void TelemetryService::update_metrics_model(const TelemetryReader& reader) {
    if (get_manager<Metric>().entry_exists(reader.get_metric_uuid())) {
        auto metric = get_manager<Metric>().get_entry_reference(reader.get_metric_uuid());
        metric->set_value(reader.get_value());
        agent_framework::model::attribute::EventData event{};
        event.set_parent(metric->get_parent_uuid());
        event.set_component(metric->get_uuid());
        event.set_type(metric->get_component());
        event.set_notification(agent_framework::model::enums::Notification::Update);
        m_notifications.push_back(std::move(event));
    }
}


void TelemetryService::update_resource_health(const TelemetryReader& reader) {
    if (!update_health<agent_framework::model::Processor>(reader, m_notifications) &&
        !update_health<agent_framework::model::Memory>(reader, m_notifications) &&
        !update_health<agent_framework::model::System>(reader, m_notifications)) {
        log_warning("telemetry", "Health not handled for " << reader.get_resource_key());
    }
}


void TelemetryService::remove_metric(const TelemetryReader& reader) {
    if (reader.fills_metric()) {
        agent_framework::model::attribute::EventData event{};
        agent_framework::module::get_manager<Metric>()
            .remove_entry(reader.get_metric_uuid(), [&event](const Metric& metric) {
                event.set_parent(metric.get_parent_uuid());
                event.set_component(metric.get_uuid());
                event.set_type(metric.get_component());
                event.set_notification(agent_framework::model::enums::Notification::Remove);
            });
        if (!event.get_component().empty()) {
            log_info("telemetry", "Removed " << Metric::get_component().to_string()
                                             << ", UUID " << event.get_component());
            m_notifications.push_back(event);
        }
    }
}


TelemetryService::~TelemetryService() {
    for (const auto& reader : get_reader_ptrs()) {
        remove_metric(*reader);
    }
    if (!m_notifications.empty()) {
        agent_framework::eventing::EventsQueue::get_instance()->push_back(m_notifications);
    }
}


TelemetryService::TelemetryService(ipmi::IpmiController& _ctrl, TelemetryReader::PtrVector&& _reader_ptrs) :
    m_metrics_processor(_ctrl, configure(std::move(_reader_ptrs))) {}


TelemetryReader::PtrVector TelemetryService::process_all_metrics() {
    /* get all metrics changed in current run */
    TelemetryReader::PtrVector changed = m_metrics_processor.read_all_metrics();

    /* try to update all changed metrics */
    for (const auto& reader : changed) {
        if (reader->fills_metric()) {
            update_metrics_model(*reader);
        }
        if (reader->fills_health()) {
            update_resource_health(*reader);
        }
    }

    /* send all collected events */
    if (m_notifications.empty()) {
        log_debug("telemetry", "No notifications collected from processing of metrics.");
    }
    else {
        agent_framework::eventing::EventsQueue::get_instance()->push_back(m_notifications);
        m_notifications.clear();
    }

    return changed;
}


const TelemetryReader::PtrVector& TelemetryService::get_reader_ptrs() const {
    return m_metrics_processor.get_reader_ptrs();
}


std::chrono::time_point<std::chrono::steady_clock> TelemetryService::get_earliest_update_time() const {
    return m_metrics_processor.get_earliest_update_time();
}


void TelemetryService::add_or_update(const Metric& metric) {
    const auto& status = agent_framework::module::get_manager<Metric>().add_or_update_entry(metric);
    agent_framework::model::attribute::EventData edat{};
    edat.set_parent(metric.get_parent_uuid());
    edat.set_component(metric.get_uuid());
    edat.set_type(metric.get_component());

    if (agent_framework::module::TableInterface::UpdateStatus::Added == status) {
        edat.set_notification(agent_framework::model::enums::Notification::Add);
        log_info("telemetry", "Added " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid()
                                       << " for " << metric.get_component_type() << " (" << metric.get_component_uuid()
                                       << ")");
        m_notifications.push_back(std::move(edat));
    }
    else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status ||
             agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
        edat.set_notification(agent_framework::model::enums::Notification::Update);
        log_info("telemetry", "Updated " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid()
                                         << " for " << metric.get_component_type() << " ("
                                         << metric.get_component_uuid() << ")");
        m_notifications.push_back(std::move(edat));
    }
    else {
        return;
    }
}


void TelemetryService::add_or_update(const MetricDefinition& metric_def) {
    const auto& status = agent_framework::module::get_manager<MetricDefinition>().add_or_update_entry(metric_def);
    agent_framework::model::attribute::EventData edat{};
    edat.set_parent(metric_def.get_parent_uuid());
    edat.set_component(metric_def.get_uuid());
    edat.set_type(metric_def.get_component());

    if (agent_framework::module::TableInterface::UpdateStatus::Added == status) {
        edat.set_notification(agent_framework::model::enums::Notification::Add);
        log_info("telemetry",
                 "Added " << MetricDefinition::get_component().to_string() << ", UUID " << metric_def.get_uuid());
        m_notifications.push_back(std::move(edat));
    }
    else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status ||
             agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
        edat.set_notification(agent_framework::model::enums::Notification::Update);
        log_info("telemetry",
                 "Updated " << MetricDefinition::get_component().to_string() << ", UUID " << metric_def.get_uuid());
        m_notifications.push_back(std::move(edat));
    }
    else {
        return;
    }
}


TelemetryReader::PtrVector TelemetryService::configure(TelemetryReader::PtrVector&& readers) {
    constexpr char DEFAULT_INTERVAL[] = "PT10S";
    const auto& config = configuration::Configuration::get_instance().to_json();

    if (!config.count("telemetry")) {
        for (auto& reader : readers) {
            reader->set_sensing_interval(DEFAULT_INTERVAL);
            reader->set_shoreup_period(DEFAULT_INTERVAL);
        }
    }
    else {
        const auto& telemetry = config.value("telemetry", json::Json());
        const auto sensing_interval = read_interval(telemetry, "defaultInterval", DEFAULT_INTERVAL);
        const auto shoreup_period_str = read_interval(telemetry, "shoreupPeriod", DEFAULT_INTERVAL);

        for (auto& reader : readers) {
            reader->set_sensing_interval(sensing_interval);
            reader->set_shoreup_period(shoreup_period_str);

            /* apply properties from the definition, by name
             * Some readers doesn't have metric definition (these are health only), so it is
             * impossible to set their properties. Only common settings apply.
             */
            if (reader->get_metric_definition().get_name().has_value() &&
                telemetry.count(reader->get_metric_definition().get_name()) &&
                telemetry[reader->get_metric_definition().get_name()].is_object()) {

                log_debug("telemetry", "Apply configuration for " << reader->get_metric_definition().get_name());
                try {
                    reader->set_metric_definition_properties(
                        telemetry[reader->get_metric_definition().get_name()]);
                }
                catch (const std::exception& e) {
                    log_error("telemetry",
                              "Cannot set all properties for " << reader->get_metric_definition().get_name()
                                                               << ": " << e.what());
                    /* ignore the exception */
                }
            }
        }
    }
    return std::move(readers);
}

} // telemetry
