/*!
 * @brief Nvme agent's TelemetryService declaration.
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
 * @file nvme/include/telemetry/telemetry_service.hpp
 */

#pragma once



#include "tools/drive_discovery/base_drive_handler.hpp"
#include "nvme_agent_context.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/metric.hpp"
#include "agent-framework/module/model/metric_definition.hpp"

#include <memory>
#include <type_traits>



namespace agent_framework {
namespace model {
class Drive;
}
}

namespace agent {
namespace nvme {
namespace telemetry {

using MetricUpdateCallback = std::function<void(agent_framework::model::Metric&,
                                                const OptionalField<tools::BaseDriveHandler::LatencyData>&,
                                                const OptionalField<tools::BaseDriveHandler::SmartData>&)>;
/*!
 * @brief Nvme telemetry service.
 */
class TelemetryService final {
public:

    /*!
     * @brief Defaulted constructor.
     */
    TelemetryService() = default;


    /*!
     * @brief Defaulted copy constructor.
     */
    TelemetryService(const TelemetryService& rhs) = default;


    /*!
     * @brief Defaulted move constructor.
     */
    TelemetryService(TelemetryService&& rhs) = default;


    /*!
     * @brief Defaulted assignment operator.
     */
    TelemetryService& operator=(const TelemetryService& rhs) = default;


    /*!
     * @brief Defaulted move assignment operator.
     */
    TelemetryService& operator=(TelemetryService&& rhs) = default;


    /*!
     * @brief Defaulted destructor.
     */
    ~TelemetryService() = default;


    /*!
     * @brief Initializes the nvme telemetry.
     *
     * @param context The context for the initialization operation.
     */
    void init(std::shared_ptr<NvmeAgentContext> context) const;


    /*!
     * @brief Updates nvme related metrics.
     *
     * @param context The context for metrics update.
     */
    void update_metrics(std::shared_ptr<NvmeAgentContext> context) const;


private:

    /*!
     * @brief Create smart metrics for the given drive.
     *
     * @param drive The model drive to create metrics for.
     * @param smart The drive's SMART data.
     */
    void create_smart_metrics(const agent_framework::model::Drive& drive,
                              const tools::BaseDriveHandler::SmartData& smart) const;


    /*!
     * @brief Build single latency metric for the given drive.
     *
     * @param drive The model drive to create metric for.
     * @param definition Metric definition of latency metric.
     * @param name The name for created metric.
     * @param latency The drive's latency data.
     * @param callback Callback for updating the metric
     */
    void build_single_latency_metric(const agent_framework::model::Drive& drive,
                                     const agent_framework::model::MetricDefinition& definition,
                                     const std::string& name,
                                     const tools::BaseDriveHandler::LatencyData& latency,
                                     const MetricUpdateCallback callback) const;


    /*!
     * @brief Create latency metrics for the given drive.
     *
     * @param drive The model drive to create metrics for.
     * @param latency The drive's Latency data.
     */
    void create_latency_metrics(const agent_framework::model::Drive& drive,
                                const tools::BaseDriveHandler::LatencyData& latency) const;


    /*!
     * @brief Registers smart metric definitions handled by this telemetry service.
     */
    void register_smart_metric_definitions() const;


    /*!
     * @brief Registers latency metric definitions handled by this telemetry service.
     */
    void register_latency_metric_definitions() const;


    /*!
     * @brief Add or update a resource to the model.
     *
     * @param resource The resource to be added or updated.
     */
    template<typename T, typename std::enable_if<std::is_base_of<agent_framework::model::Resource, T>{}, int>::type = 0>
    void add_or_update(const T& resource) const {
        const auto& status = agent_framework::module::get_manager<T>().add_or_update_entry(resource);
        agent_framework::model::attribute::EventData edat{};
        edat.set_parent(resource.get_parent_uuid());
        edat.set_component(resource.get_uuid());
        edat.set_type(resource.get_component());

        if (agent_framework::module::TableInterface::UpdateStatus::Added == status) {
            edat.set_notification(agent_framework::model::enums::Notification::Add);
            log_info("telemetry",
                     "Added " << T::get_component().to_string() << ", UUID " << resource.get_uuid());
            agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
        }
        else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status
                 || agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
            edat.set_notification(agent_framework::model::enums::Notification::Update);
            log_info("telemetry",
                     "Updated " << T::get_component().to_string() << ", UUID " << resource.get_uuid());
            agent_framework::eventing::EventsQueue::get_instance()->push_back(std::move(edat));
        }
        else {
            return;
        }
    }


    /*!
     * @brief Update drive health.
     *
     * @param model_drive The drive to be updated.
     * @param smart The drive's SMART data.
     */
    void update_drive_health(agent_framework::model::Drive& model_drive,
                             const tools::BaseDriveHandler::SmartData& smart) const;


    /*!
     * @brief Sets drive's health to critical
     * @param drive_uuid Uuid of the drive to be changed
     */
    void set_drive_health_to_critical(const Uuid& drive_uuid) const;
};


template<>
void TelemetryService::add_or_update(const agent_framework::model::Metric& metric) const;

} // namespace telemetry
} // namespace nvme
} // namespace agent
