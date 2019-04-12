/*!
 * @brief Implementation of nvme agent's TelemetryService class
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
#include "tree_stability/nvme_stabilizer.hpp"
#include "discovery/discovery_manager.hpp"
#include "telemetry/nvme_metric_definitions.hpp"



namespace {

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::nvme::telemetry;
using namespace agent::nvme::tools;

using SmartData = BaseDriveHandler::SmartData;
using LatencyData = BaseDriveHandler::LatencyData;
using CriticalWarningHandlers = std::vector<std::function<void(std::stringstream&, attribute::Status&, const SmartData&)>>;

static std::map<Uuid, MetricUpdateCallback> metric_to_callback_map{};

constexpr uint8_t SPARE_WARNING_MASK = 1u;
constexpr uint8_t TEMPERATURE_WARNING_MASK = 1u << 1;
constexpr uint8_t MEDIA_ERROR_MASK = 1u << 2;
constexpr uint8_t READ_ONLY_MASK = 1u << 3;
constexpr uint8_t VOLATILE_ERROR_MASK = 1u << 4;
constexpr int DEFAULT_UNIT_SIZE = 512 * 1000; // as per NVMe spec



const MetricDefinition& get_latency_metric_definition() {
    static MetricDefinition ret{
        LATENCY_BUCKETS
    };

    return ret;
}


const std::vector<MetricDefinition>& get_smart_metric_definitions() {
    static std::vector<MetricDefinition> ret{
        TEMPERATURE_KELVIN,
        UNIT_SIZE_BYTES,
        UNITS_READ,
        UNITS_WRITTEN,
        HOST_READ_COMMANDS,
        HOST_WRITE_COMMANDS,
        POWER_CYCLES,
        POWER_ON_HOURS,
        CONTROLLER_BUSY_TIME_MINUTES,
        AVAILABLE_SPARE_PERCENTAGE,
        PREDICTED_MEDIA_LIFE_USED_PERCENT,
        UNSAFE_SHUTDOWNS,
        MEDIA_ERRORS
    };

    return ret;
}


const std::map<std::string, MetricUpdateCallback> get_smart_callbacks() {
    static std::map<std::string, MetricUpdateCallback> ret
        {
            {Constants::TEMPERATURE_KELVIN_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().composite_temperature); }
            },
            {Constants::UNIT_SIZE_BYTES_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>&) { metric.set_value(DEFAULT_UNIT_SIZE); }
            },
            {Constants::UNITS_READ_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().data_unit_read); }
            },
            {Constants::UNITS_WRITTEN_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().data_unit_written); }
            },
            {Constants::HOST_READ_COMMANDS_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().host_read_commands); }
            },
            {Constants::HOST_WRITE_COMMANDS_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().host_write_commands); }
            },
            {Constants::POWER_CYCLES_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().power_cycles); }
            },
            {Constants::POWER_ON_HOURS_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().power_on_hours); }
            },
            {Constants::CONTROLLER_BUSY_TIME_MINUTES_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().controller_busy_time); }
            },
            {Constants::AVAILABLE_SPARE_PERCENTAGE_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().available_spare); }
            },
            {Constants::PREDICTED_MEDIA_LIFE_USED_PERCENT_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().percentage_used); }
            },
            {Constants::UNSAFE_SHUTDOWNS_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().unsafe_shutdowns); }
            },
            {Constants::MEDIA_ERRORS_JSON_PTR,
                [](Metric& metric, const OptionalField<LatencyData>&,
                   const OptionalField<SmartData>& smart) { metric.set_value(smart.value().media_data_integrity_errors); }
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


std::vector<std::string> make_bucket_1st_name(const BaseDriveHandler::LatencyData::HistogramData& histogramData) {

    std::vector<std::string> metric_names{};
    std::uint32_t min_range = histogramData.group_1st.min_range_us / 1000;
    std::uint32_t max_range = min_range + Constants::RANGE;

    std::uint32_t step = histogramData.group_1st.step_us;
    for (std::uint32_t bucket = 0; bucket < histogramData.group_1st.buckets.size() - 1; bucket++) {
        metric_names.push_back(Constants::FROM + std::to_string(min_range) + Constants::TO + std::to_string(max_range) + Constants::MICRO_SECONDS);
        min_range = max_range + 1;
        max_range = max_range + step;
    }
    metric_names.push_back(Constants::FROM + std::to_string(min_range) + Constants::TO + std::to_string(max_range + 1) + Constants::MICRO_SECONDS);
    return metric_names;
}


std::vector<std::string> make_bucket_2nd_name(const BaseDriveHandler::LatencyData::HistogramData& histogramData) {

    std::vector<std::string> metric_names{};
    std::uint32_t min_range = histogramData.group_2nd.min_range_us / 1000;
    std::uint32_t max_range = min_range + 1;

    std::uint32_t step = histogramData.group_2nd.step_us / 1000;
    for (std::uint32_t bucket = 0; bucket < histogramData.group_2nd.buckets.size(); bucket++) {
        metric_names.push_back(
            Constants::FROM + std::to_string(min_range) + Constants::TO + std::to_string(max_range) + Constants::MILLI_SECONDS);
        min_range = max_range;
        max_range = max_range + step;
    }
    return metric_names;
}


std::vector<std::string> make_bucket_3rd_name(const BaseDriveHandler::LatencyData::HistogramData& histogramData) {

    std::vector<std::string> metric_names{};
    std::uint32_t min_range = histogramData.group_3rd.min_range_us / 1000;
    std::uint32_t max_range = min_range + Constants::RANGE;
    std::uint32_t step = histogramData.group_3rd.step_us / 1000;
    for (std::uint32_t bucket = 0; bucket < histogramData.group_3rd.buckets.size(); bucket++) {
        metric_names.push_back(
            Constants::FROM + std::to_string(min_range) + Constants::TO + std::to_string(max_range) + Constants::MILLI_SECONDS);
        min_range = max_range + 1;
        max_range = max_range + step;
    }
    return metric_names;
}


std::string make_bucket_4th_5th_name(LatencyData::HistogramGroup historgram_group) {
    return Constants::FROM + std::to_string(historgram_group.min_range_us / 1000) + Constants::TO +
           std::to_string(historgram_group.max_range_us / 1000) + Constants::MILLI_SECONDS;
}


std::string make_bucket_6th_name(const BaseDriveHandler::LatencyData& latency) {
    return Constants::FROM + std::to_string(latency.read_histogram.group_6th.min_range_us / 1000) + Constants::MILLI_SECONDS;
}

}


void TelemetryService::init(std::shared_ptr<NvmeAgentContext> context) const {
    register_smart_metric_definitions();
    register_latency_metric_definitions();

    auto model_drives = get_manager<Drive>().get_entries();
    for (const auto& model_drive : model_drives) {
        try {
            if (!model_drive.get_name().has_value()) {
                throw std::runtime_error("Drive's name is unknown.");
            }

            // create the drive metrics
            auto handler = context->drive_handler_factory->get_handler(model_drive.get_name());
            if (handler) {
                handler->load();
                log_debug("telemetry", "Creating Smart Metrics.");
                create_smart_metrics(model_drive, handler->load_smart_info());
                log_debug("telemetry", "Creating Latency Metrics.");
                create_latency_metrics(model_drive, handler->load_latency_histogram());
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
            if (!model_drive.get_name().has_value()) {
                throw std::runtime_error("Drive's name is unknown.");
            }
            // get metrics for this particular drive
            const auto metric_keys = get_manager<Metric>().get_keys([&model_drive](const Metric& metric) {
                return metric.get_component_uuid() == model_drive.get_uuid();
            });

            auto handler = context->drive_handler_factory->get_handler(model_drive.get_name());
            if (!handler) {
                log_error("telemetry", "Unable to get handler for the drive!");
                set_drive_health_to_critical(model_drive.get_uuid());
                throw std::runtime_error("Unable to update metrics.");
            }

            handler->load();
            auto smart_data = handler->load_smart_info();
            auto latency_histogram = handler->load_latency_histogram();

            // new drive, create metrics
            if (metric_keys.empty()) {
                create_smart_metrics(model_drive, smart_data);
                create_latency_metrics(model_drive, latency_histogram);
            }
            else {
                try {
                    // update metrics
                    for (const auto& key : metric_keys) {
                        auto metric_ref = get_manager<Metric>().get_entry_reference(key);
                        auto& metric = metric_ref.get_raw_ref();
                        metric_to_callback_map[metric.get_uuid()](metric, latency_histogram, smart_data);
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
            log_warning("telemetry", "Cannot update metrics: " << e.what());
        }
    }

    // TODO handle drive removal
}


void TelemetryService::create_smart_metrics(const Drive& drive, const SmartData& smart) const {
    auto metric_types = get_smart_metric_definitions();

    // get keys for SMART metric definitions
    auto keys = get_manager<MetricDefinition>().get_keys([&metric_types](const MetricDefinition& definition) {
        return std::any_of(std::begin(metric_types), std::end(metric_types),
                           [&definition](MetricDefinition& metric_type) {
                               return definition.get_metric_jsonptr() == metric_type.get_metric_jsonptr();
                           });
    });

    // check if all metric definitions are present
    if (keys.size() != metric_types.size()) {
        std::stringstream err{"Invalid number of metric definitions found "};
        err << keys.size() << "/" << metric_types.size();
        throw std::runtime_error(err.str());
    }

    auto metric_callbacks = get_smart_callbacks();
    NvmeStabilizer nts{};
    // for each drive metric definition, create a corresponding metric
    for (const auto& key : keys) {
        try {
            auto definition = get_manager<MetricDefinition>().get_entry(key);

            Metric metric{};
            // update from model
            metric.set_component_type(drive.get_component());
            metric.set_component_uuid(drive.get_uuid());
            // update from metric definition
            metric.set_metric_definition_uuid(definition.get_uuid());
            metric.set_name(definition.get_metric_jsonptr());
            metric_callbacks[definition.get_metric_jsonptr()](metric, {}, smart);

            nts.stabilize(metric, static_cast<const Resource&>(drive));
            add_or_update(metric);
            metric_to_callback_map[metric.get_uuid()] = metric_callbacks[definition.get_metric_jsonptr()];
        }
        catch (const std::exception& e) {
            log_warning("telemetry", "Cannot create smart metric: " << e.what());
        }
    }
}


void TelemetryService::build_single_latency_metric(const Drive& drive, const MetricDefinition& definition,
                                                   const std::string& name,
                                                   const LatencyData& latency,
                                                   MetricUpdateCallback callback) const {
    NvmeStabilizer nts{};
    // create a corresponding metric for drive metric definition
    try {
        Metric metric{};
        // update from model
        metric.set_component_type(drive.get_component());
        metric.set_component_uuid(drive.get_uuid());
        // update from metric definition
        metric.set_metric_definition_uuid(definition.get_uuid());
        metric.set_name(name);
        callback(metric, latency, {});

        nts.stabilize(metric, static_cast<const Resource&>(drive));
        add_or_update(metric);
        metric_to_callback_map[metric.get_uuid()] = callback;
    }
    catch (const std::exception& e) {
        log_warning("telemetry", "Cannot create latency metric: " << e.what());
    }
}


void TelemetryService::create_latency_metrics(const Drive& drive, const LatencyData& latency) const {
    auto metric_type = get_latency_metric_definition();
    // get metric definition for latency metric
    auto definition = get_manager<MetricDefinition>().get_entry(metric_type.get_uuid());

    // create latency metrics for all histogram groups
    auto buckets_names = make_bucket_1st_name(latency.read_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::READS_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->read_histogram.group_1st.buckets[bucket]);
                                    });
    }

    buckets_names = make_bucket_2nd_name(latency.read_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::READS_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->read_histogram.group_2nd.buckets[bucket]);
                                    });
    }

    buckets_names = make_bucket_3rd_name(latency.read_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::READS_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->read_histogram.group_3rd.buckets[bucket]);
                                    });
    }

    build_single_latency_metric(drive, definition,
                                Constants::READS_HISTOGRAM + make_bucket_4th_5th_name(latency.read_histogram.group_4th),
                                latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->read_histogram.group_4th.buckets[0]);
                                });

    build_single_latency_metric(drive, definition,
                                Constants::READS_HISTOGRAM + make_bucket_4th_5th_name(latency.read_histogram.group_5th),
                                latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->read_histogram.group_5th.buckets[0]);
                                });

    build_single_latency_metric(drive, definition, Constants::READS_HISTOGRAM + make_bucket_6th_name(latency), latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->read_histogram.group_6th.buckets[0]);
                                });

    buckets_names = make_bucket_1st_name(latency.write_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::WRITES_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->write_histogram.group_1st.buckets[bucket]);
                                    });
    }

    buckets_names = make_bucket_2nd_name(latency.write_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::WRITES_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->write_histogram.group_2nd.buckets[bucket]);
                                    });
    }

    buckets_names = make_bucket_3rd_name(latency.write_histogram);
    for (std::uint32_t bucket = 0; bucket < buckets_names.size(); bucket++) {
        build_single_latency_metric(drive, definition, Constants::WRITES_HISTOGRAM + buckets_names[bucket], latency,
                                    [bucket](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                             const OptionalField<SmartData>&) {
                                        metric.set_value(latency_data->write_histogram.group_3rd.buckets[bucket]);
                                    });
    }

    build_single_latency_metric(drive, definition,
                                Constants::WRITES_HISTOGRAM +
                                make_bucket_4th_5th_name(latency.write_histogram.group_4th),
                                latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->write_histogram.group_4th.buckets[0]);
                                });

    build_single_latency_metric(drive, definition,
                                Constants::WRITES_HISTOGRAM +
                                make_bucket_4th_5th_name(latency.write_histogram.group_5th),
                                latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->write_histogram.group_5th.buckets[0]);
                                });

    build_single_latency_metric(drive, definition, Constants::WRITES_HISTOGRAM + make_bucket_6th_name(latency), latency,
                                [](Metric& metric, const OptionalField<LatencyData>& latency_data,
                                   const OptionalField<SmartData>&) {
                                    metric.set_value(latency_data->write_histogram.group_6th.buckets[0]);
                                });
}


void TelemetryService::register_smart_metric_definitions() const {
    NvmeStabilizer nts{};
    auto definitions = get_smart_metric_definitions();
    std::for_each(std::begin(definitions), std::end(definitions),
                  [this, &nts](MetricDefinition& definition) {
                      add_or_update(definition);
                      nts.stabilize(definition);
                  });
}


void TelemetryService::register_latency_metric_definitions() const {
    NvmeStabilizer nts{};
    auto definition = get_latency_metric_definition();
    add_or_update(definition);
    nts.stabilize(definition);
}


template<>
void TelemetryService::add_or_update(const Metric& metric) const {
    const auto& status = get_manager<Metric>().add_or_update_entry(metric);
    attribute::EventData edat{};
    edat.set_parent(metric.get_parent_uuid());
    edat.set_component(metric.get_uuid());
    edat.set_type(metric.get_component());

    if (TableInterface::UpdateStatus::Added == status) {
        edat.set_notification(enums::Notification::Add);
        log_info("telemetry",
                 "Added " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid() <<
                          " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");
        agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
    }
    else if (TableInterface::UpdateStatus::StatusChanged == status
             || TableInterface::UpdateStatus::Updated == status) {
        edat.set_notification(enums::Notification::Update);
        log_info("telemetry",
                 "Updated " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid() <<
                            " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");
        agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
    }
    else {
        return;
    }
}


void TelemetryService::update_drive_health(Drive& model_drive, const BaseDriveHandler::SmartData& smart) const {
    std::stringstream ss;
    ss << "Drive " << model_drive.get_uuid() << ": ";
    auto drive_status = model_drive.get_status();
    auto old_health = drive_status.get_health();

    const auto& handlers = get_health_handlers();
    // process drive health
    std::for_each(std::begin(handlers), std::end(handlers),
                  [&ss, &drive_status, &smart](CriticalWarningHandlers::const_reference ref) {
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
