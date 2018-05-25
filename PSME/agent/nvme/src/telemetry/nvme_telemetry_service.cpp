/*!
 * @brief Implementation of nvme agent's TelemetryService class
 *
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file telemetry/nvme_telemetry_service.cpp
 */

#include "telemetry/nvme_telemetry_service.hpp"
#include "telemetry/nvme_metric_definitions.hpp"
#include "agent-framework/module/model/drive.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "discovery/discovery_manager.hpp"

#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <utility>
#include <type_traits>

namespace {

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::nvme::telemetry;
using SmartData = agent::nvme::tools::BaseDriveHandler::SmartData;
using SmartCallback = std::function<void(Metric&, const SmartData&)>;
using MetricDefinitions = std::vector<std::pair<MetricDefinition, SmartCallback>>;
using CriticalWarningHandlers = std::vector<std::function<void(std::stringstream&, attribute::Status&, const SmartData&)>>;

constexpr uint8_t SPARE_WARNING_MASK = 1u;
constexpr uint8_t TEMPERATURE_WARNING_MASK = 1u << 1;
constexpr uint8_t MEDIA_ERROR_MASK = 1u << 2;
constexpr uint8_t READ_ONLY_MASK = 1u << 3;
constexpr uint8_t VOLATILE_ERROR_MASK = 1u << 4;
constexpr int DEFAULT_UNIT_SIZE = 512 * 1000; // as per NVMe spec

const MetricDefinitions& get_smart_metric_definitions() {
    static MetricDefinitions ret{
        {
            TEMPERATURE_KELVIN, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.composite_temperature);
            }
        },
        {
            UNIT_SIZE_BYTES, [](Metric& metric, const SmartData&) {
                metric.set_value(DEFAULT_UNIT_SIZE);
            }
        },
        {
            UNITS_READ, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.data_unit_read);
            }
        },
        {
            UNITS_WRITTEN, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.data_unit_written);
            }
        },
        {
            HOST_READ_COMMANDS, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.host_read_commands);
            }
        },
        {
            HOST_WRITE_COMMANDS, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.host_write_commands);
            }
        },
        {
            POWER_CYCLES, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.power_cycles);
            }
        },
        {
            POWER_ON_HOURS, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.power_on_hours);
            }
        },
        {
            CONTROLLER_BUSY_TIME_MINUTES, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.controller_busy_time);
            }
        },
        {
            AVAILABLE_SPARE_PERCENTAGE, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.available_spare);
            }
        },
        {
            PREDICTED_MEDIA_LIFE_USED_PERCENT, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.percentage_used);
            }
        },
        {
            UNSAFE_SHUTDOWNS, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.unsafe_shutdowns);
            }
        },
        {
            MEDIA_ERRORS, [](Metric& metric, const SmartData& smart) {
                metric.set_value(smart.media_data_integrity_errors);
            }
        }
    };

    return ret;
}

const CriticalWarningHandlers& get_health_handlers() {
    // has to be in ascending health
    static CriticalWarningHandlers ret{
            [](std::stringstream& ss, attribute::Status& status, const SmartData& sd) {
                if (sd.critical_warnings & SPARE_WARNING_MASK) {
                    status.set_health(enums::Health::Warning);
                    ss << " Available spare space has fallen below the threshold.";
                }
        },
            [](std::stringstream& ss, attribute::Status& status, const SmartData& sd) {
                if (sd.critical_warnings & TEMPERATURE_WARNING_MASK) {
                    status.set_health(enums::Health::Warning);
                    ss << " Temperature is above an over temperature threshold or below an under temperature threshold.";
                }
        },
            [](std::stringstream& ss, attribute::Status& status, const SmartData& sd) {
                if (sd.critical_warnings & MEDIA_ERROR_MASK) {
                    status.set_health(enums::Health::Critical);
                    ss << " NVM subsystem reliability has been degraded.";
                }
        },
            [](std::stringstream& ss, attribute::Status& status, const SmartData& sd) {
                if (sd.critical_warnings & READ_ONLY_MASK) {
                    status.set_health(enums::Health::Critical);
                    ss << " Media has been placed in read only mode.";
                }
        },
            [](std::stringstream& ss, attribute::Status& status, const SmartData& sd) {
                if (sd.critical_warnings & VOLATILE_ERROR_MASK) {
                    status.set_health(enums::Health::Critical);
                    ss << " The volatile memory backup device has failed.";
                }
        }
    };

    return ret;
}

std::string get_drive(const Drive& model_drive) {

    auto identifiers = model_drive.get_identifiers();
    // find path identifier
    auto found_id = std::find_if(std::begin(identifiers), std::end(identifiers),
        [](const attribute::Identifier& id) {
        bool ret = false;
        if (id.get_durable_name_format() == enums::IdentifierType::SystemPath) {
            ret = true;
        }
        return ret;
    });

    if (found_id == std::end(identifiers)) {
        std::stringstream ss;
        ss << "Drive " << model_drive.get_uuid() << " system path not found";
        throw std::runtime_error(ss.str());
    }

    const auto& path = found_id->get_durable_name();
    // trivial basename
    return path->substr(path->find_last_of('/') + 1);
}

void update_metric_value(agent_framework::model::Metric& metric,
    const agent::nvme::tools::BaseDriveHandler::SmartData& smart) {
    const auto& metric_types = get_smart_metric_definitions();
    auto found_metric = std::find_if(std::begin(metric_types), std::end(metric_types),
        [&metric](MetricDefinitions::const_reference elem) {
            return elem.first.get_uuid() == metric.get_metric_definition_uuid();
        });

    // metric not found
    if (found_metric == metric_types.end()) {
        std::stringstream ss{"Metric "};
        ss << metric.get_name() << " not found";
        throw std::runtime_error(ss.str());
    }

    // update the metric through the callback
    found_metric->second(metric, smart);
}

}

void TelemetryService::init(std::shared_ptr<NvmeAgentContext> context) const {
    register_metric_definitions();

    auto model_drives = get_manager<Drive>().get_entries();
    for (const auto& model_drive : model_drives) {
        try {
            auto drive = get_drive(model_drive);
            // create the drive metrics
            auto handler = context->drive_handler_factory->get_handler(drive);
            if (handler) {
                create_drive_metrics(model_drive, handler->get_smart_info());
            }
            else {
                // if handler is not available - do not add metric definitions
                // they may be added the first time smart is read, therefore state will be automatically changed from critical
                log_error("telemetry", "Unable to get handler for the drive!");
                set_drive_health_to_critical(model_drive.get_uuid());
            }
        }
        catch (const std::exception& e) {
            log_warning("telemetry", e.what());
        }
    }
}

void TelemetryService::update_metrics(std::shared_ptr<NvmeAgentContext> context) const {
    auto model_drives = get_manager<Drive>().get_entries();
    for (auto& model_drive : model_drives) {
        try {
            // get drive name
            auto drive = get_drive(model_drive);
            // get metrics for this particular drive
            const auto metric_keys = get_manager<Metric>().get_keys([&model_drive](const Metric& metric) {
                return metric.get_component_uuid() == model_drive.get_uuid();
            });

            auto handler = context->drive_handler_factory->get_handler(drive);
            if (!handler) {
                log_error("telemetry", "Unable to get handler for the drive!");
                set_drive_health_to_critical(model_drive.get_uuid());
                throw std::runtime_error("Unable to update smart data");
            }
            auto smart_data = handler->get_smart_info();

            // new drive, create metrics
            if (metric_keys.empty()) {
                create_drive_metrics(model_drive, smart_data);
            }
            else {
                try {
                    // update metrics
                    for (const auto& key : metric_keys) {
                        auto tmp = get_manager<Metric>().get_entry_reference(key);
                        auto& metric = tmp.get_raw_ref();

                        update_metric_value(metric, smart_data);
                        add_or_update(metric);
                    }

                    update_drive_health(model_drive, smart_data);
                    add_or_update(model_drive);
                }
                catch (const std::exception& e) {
                    log_warning("telemetry", e.what());
                }
            }
        }
        catch (const std::exception& e) {
            log_warning("telemetry", e.what());
        }
    }

    // TODO handle drive removal
}

void TelemetryService::create_drive_metrics(const Drive& drive, const SmartData& smart) const {
    const auto& metric_types = get_smart_metric_definitions();
    // get keys for SMART metric definitions
    auto keys = get_manager<MetricDefinition>().get_keys([&metric_types](const MetricDefinition& definition) {
        return std::any_of(std::begin(metric_types), std::end(metric_types),
            [&definition](MetricDefinitions::const_reference metric_type) {
                return definition.get_metric_jsonptr() == metric_type.first.get_metric_jsonptr();
            });
    });

    // check if all metric definitions are present
    if (keys.size() != metric_types.size()) {
        std::stringstream err{"Invalid number of metric definitions found "};
        err << keys.size() << "/" << metric_types.size();
        throw std::runtime_error(err.str());
    }

    NvmeStabilizer nts{};
    // for each drive metric definition, create a corresponding metric
    for (const auto& key : keys) {
        try {
            auto definition = get_manager<MetricDefinition>().get_entry_reference(key);

            Metric metric{};
            // update from model
            metric.set_component_type(drive.get_component());
            metric.set_component_uuid(drive.get_uuid());
            // update from metric definition
            metric.set_metric_definition_uuid(definition->get_uuid());
            metric.set_name(definition->get_metric_jsonptr());
            update_metric_value(metric, smart);
            add_or_update(metric);
            (void) nts.stabilize(metric, static_cast<const agent_framework::model::Resource&>(drive));
        }
        catch (const std::exception& e) {
            log_warning("telemetry", e.what());
        }
    }
}

void TelemetryService::register_metric_definitions() const {
    NvmeStabilizer nts{};
    auto definitions = get_smart_metric_definitions();
    std::for_each(std::begin(definitions), std::end(definitions),
        [this, &nts](MetricDefinitions::reference definition) {
            add_or_update(definition.first);
            (void) nts.stabilize(definition.first);
        });
}

template<>
void TelemetryService::add_or_update(const Metric& metric) const {
    const auto& status = agent_framework::module::get_manager<Metric>().add_or_update_entry(metric);
    agent_framework::eventing::EventData edat{};
    edat.set_parent(metric.get_parent_uuid());
    edat.set_component(metric.get_uuid());
    edat.set_type(metric.get_component());

    if (agent_framework::module::TableInterface::UpdateStatus::Added == status) {
        edat.set_notification(agent_framework::eventing::Notification::Add);
        log_info("telemetry",
            "Added " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid() <<
            " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");
        agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
    }
    else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status
        || agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
        edat.set_notification(agent_framework::eventing::Notification::Update);
        log_info("telemetry",
            "Updated " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid() <<
            " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");
        agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
    }
    else {
        return;
    }
}

void TelemetryService::update_drive_health(Drive& model_drive, const tools::BaseDriveHandler::SmartData& smart) const {
    std::stringstream ss;
    ss << "Drive " << model_drive.get_uuid() << ": ";
    auto drive_status = model_drive.get_status();
    auto old_health = drive_status.get_health();

    const auto& handlers = get_health_handlers();
    // process drive health
    std::for_each(std::begin(handlers), std::end(handlers),
        [&ss, &drive_status, &smart](CriticalWarningHandlers::const_reference ref){
            ref(ss, drive_status, smart);
    });

    // check if health changed and update model
    auto new_health = drive_status.get_health();
    if ((!old_health.has_value() && new_health.has_value()) || old_health != new_health) {
        log_warning("telemetry", ss.str());
        model_drive.set_status(drive_status);
    }
}

void TelemetryService::set_drive_health_to_critical(const Uuid& drive_uuid) const {
    auto drive_ref = get_manager<Drive>().get_entry_reference(drive_uuid);
    auto status = drive_ref->get_status();
    status.set_health(enums::Health::Critical);
    drive_ref->set_status(status);
}
