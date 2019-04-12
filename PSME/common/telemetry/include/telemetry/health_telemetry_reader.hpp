/*!
 * @brief Class to handle health state of the resource
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
 * @file health_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"
#include "ipmi/sel_record.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace telemetry {

class HealthTelemetryReader : public TelemetryReader {
public:
    using Event = std::pair<ipmi::SelRecordGeneric::EventSensorType, const char*>;

    /*!
     * @brief Base class for event list specification
     */
    class EventList : public std::vector<Event> {
    public:
        enum class EventType {
            CRITICAL,
            WARNING
        };

        /*!
         * @return Level for events kept in the collection
         */
        virtual EventType get_event_type() const = 0;

        EventList(const EventList&) = default;

        virtual ~EventList();

    protected:
        /*!
         * @brief Default constructor with initialization list
         * @param events to construct collection
         */
        EventList(std::initializer_list<Event> events) : std::vector<Event>(events) { }
        EventList() = delete;
    };

    /*!
     * @brief Event list for warnings
     */
    class Warnings final : public EventList {
    public:
        Warnings(std::initializer_list<Event> events) : EventList(events) { }

        ~Warnings() override;

        Warnings(const Warnings&) = default;

        EventType get_event_type() const override {
            return EventType::WARNING;
        }
    };

    /*!
     * @brief Event list for critical events
     */
    class Critical final : public EventList {
    public:
        Critical(std::initializer_list<Event> events) : EventList(events) { }

        ~Critical() override;

        Critical(const Critical&) = default;

        EventType get_event_type() const override {
            return EventType::CRITICAL;
        }
    };

    /*!
     * @brief Get reader id for the Grantley temperature readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    /*!
     * @brief Constructor for health-only reader
     * @param _resource_key resource to be handled
     * @param _metric_definition metric definition instance to be used
     * @param warning_events events which cause warning state
     * @param critical_events events which cause critical state
     */
    HealthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                          Warnings warning_events, Critical critical_events = {});

    /*!
     * @brief Constructor for health-only reader
     * @param _resource_key resource to be handled
     * @param _metric_definition metric definition instance to be used
     * @param warning_events events which cause warning state
     * @param critical_events events which cause critical state
     */
    HealthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                          Critical critical_events, Warnings warning_events = {});

    /*!
     * @brief Constructor for health-only reader
     * @param _resource_key resource to be handled
     * @param warning_events events which cause warning state
     * @param critical_events events which cause critical state
     */
    HealthTelemetryReader(ResourceInstance _resource_key, Warnings warning_events, Critical critical_events = {});

    /*!
     * @brief Constructor for health-only reader
     * @param _resource_key resource to be handled
     * @param warning_events events which cause warning state
     * @param critical_events events which cause critical state
     */
    HealthTelemetryReader(ResourceInstance _resource_key, Critical critical_events, Warnings warning_events = {});

    std::string get_info() const override { return "Health"; }

    /*!
     * @brief Get list of events which makes 'warning' in health
     * @return List of event identifiers
     */
    const EventList& get_warning_events() const {
        return m_warning_events;
    }

    /*!
     * @brief Get list of events which makes 'critical' in health
     * @return List of event identifiers
     */
    const EventList& get_critical_events() const {
        return m_critical_events;
    }

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    bool is_valid(Context::Ptr context) const override;

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;

private:
    /*!
     * @brief set the names of events in "discreteValues" property in metric definition
     */
    void fill_discreete_values();

    Warnings m_warning_events;
    Critical m_critical_events;
};

}
