/*!
 * @brief Sel reader class definition
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file sel_reader.cpp
 */



#include "log_service/sel_reader.hpp"
#include "ipmi/command/generic/sel/get_sel_info.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"
#include "agent-framework/module/utils/time.hpp"
#include "agent-framework/eventing/events_queue.hpp"


using namespace agent_framework::model;
using namespace ipmi;

using agent_framework::module::get_manager;
using agent_framework::model::utils::to_hex_string;


const std::map<SELReader::IpmiSensorType, SELReader::RedfishSensorType> SELReader::m_ipmi_redfish_sensor_map{
    {SELReader::IpmiSensorType::TEMPERATURE,                   SELReader::RedfishSensorType::Temperature},
    {SELReader::IpmiSensorType::VOLTAGE,                       SELReader::RedfishSensorType::Voltage},
    {SELReader::IpmiSensorType::CURRENT,                       SELReader::RedfishSensorType::Current},
    {SELReader::IpmiSensorType::FAN,                           SELReader::RedfishSensorType::Fan},
    {SELReader::IpmiSensorType::PHYSICAL_SECURITY,             SELReader::RedfishSensorType::PhysicalChassisSecurity},
    {SELReader::IpmiSensorType::PERFORM_SECURITY_VIOLATION,    SELReader::RedfishSensorType::PlatformSecurityViolationAttempt},
    {SELReader::IpmiSensorType::PROCESSOR,                     SELReader::RedfishSensorType::Processor},
    {SELReader::IpmiSensorType::POWER_SUPPLY,                  SELReader::RedfishSensorType::PowerSupplyOrConverter},
    {SELReader::IpmiSensorType::POWER_UNIT,                    SELReader::RedfishSensorType::PowerUnit},
    {SELReader::IpmiSensorType::COOLING_DEVICE,                SELReader::RedfishSensorType::CoolingDevice},
    {SELReader::IpmiSensorType::OTHER_UNIT_BASED_SENSOR,       SELReader::RedfishSensorType::OtherUnitsBasedSensor},
    {SELReader::IpmiSensorType::MEMORY,                        SELReader::RedfishSensorType::Memory},
    {SELReader::IpmiSensorType::DRIVE_SLOT,                    SELReader::RedfishSensorType::DriveSlotOrBay},
    {SELReader::IpmiSensorType::POST_MEMORY_RESIZE,            SELReader::RedfishSensorType::POSTMemoryResize},
    {SELReader::IpmiSensorType::SEL_LOGGING,                   SELReader::RedfishSensorType::EventLoggingDisabled},
    {SELReader::IpmiSensorType::WATCHDOG1,                     SELReader::RedfishSensorType::Watchdog},
    {SELReader::IpmiSensorType::SYSTEM_EVENT,                  SELReader::RedfishSensorType::SystemEvent},
    {SELReader::IpmiSensorType::CRITICAL_INTERRUPT,            SELReader::RedfishSensorType::CriticalInterrupt},
    {SELReader::IpmiSensorType::BUTTON,                        SELReader::RedfishSensorType::ButtonOrSwitch},
    {SELReader::IpmiSensorType::MODULE_BOARD,                  SELReader::RedfishSensorType::ModuleOrBoard},
    {SELReader::IpmiSensorType::COPROCESSOR,                   SELReader::RedfishSensorType::Processor},
    {SELReader::IpmiSensorType::ADDIN_CARD,                    SELReader::RedfishSensorType::AddInCard},
    {SELReader::IpmiSensorType::CHASSIS,                       SELReader::RedfishSensorType::Chassis},
    {SELReader::IpmiSensorType::CHIPSET,                       SELReader::RedfishSensorType::ChipSet},
    {SELReader::IpmiSensorType::OTHER_FRU,                     SELReader::RedfishSensorType::OtherFRU},
    {SELReader::IpmiSensorType::INTERCONNECT,                  SELReader::RedfishSensorType::CableOrInterconnect},
    {SELReader::IpmiSensorType::TERMINATOR,                    SELReader::RedfishSensorType::Terminator},
    {SELReader::IpmiSensorType::SYSTEM_BOOT_RESTART_INITIATED, SELReader::RedfishSensorType::SystemBootOrRestart},
    {SELReader::IpmiSensorType::BOOT_ERROR,                    SELReader::RedfishSensorType::BootError},
    {SELReader::IpmiSensorType::OS_BOOT_STATUS,                SELReader::RedfishSensorType::BaseOSBootOrInstallationStatus},
    {SELReader::IpmiSensorType::OS_STOP_SHUTDOWN,              SELReader::RedfishSensorType::OSStopOrShutdown},
    {SELReader::IpmiSensorType::CONNECTOR,                     SELReader::RedfishSensorType::SlotOrConnector},
    {SELReader::IpmiSensorType::SYSTEM_ACPI_POWER_STATE,       SELReader::RedfishSensorType::SystemACPIPowerState},
    {SELReader::IpmiSensorType::WATCHDOG2,                     SELReader::RedfishSensorType::Watchdog},
    {SELReader::IpmiSensorType::PLATFORM_ALERT,                SELReader::RedfishSensorType::PlatformAlert},
    {SELReader::IpmiSensorType::ENTITY_PRESENCE,               SELReader::RedfishSensorType::EntityPresence},
    {SELReader::IpmiSensorType::MONITOR_ASIC,                  SELReader::RedfishSensorType::MonitorASICOrIC},
    {SELReader::IpmiSensorType::LAN,                           SELReader::RedfishSensorType::LAN},
    {SELReader::IpmiSensorType::MANAGEMENT_SUBSYSTEM_HEALTH,   SELReader::RedfishSensorType::ManagementSubsystemHealth},
    {SELReader::IpmiSensorType::BATTERY,                       SELReader::RedfishSensorType::Battery},
    {SELReader::IpmiSensorType::SESSION_AUDIT,                 SELReader::RedfishSensorType::SessionAudit},
    {SELReader::IpmiSensorType::VERSION_CHANGE,                SELReader::RedfishSensorType::VersionChange},
    {SELReader::IpmiSensorType::FRU_STATE,                     SELReader::RedfishSensorType::FRUState}
};


void SELReader::read(agent::compute::Bmc& bmc) {
    auto ipmi = bmc.ipmi();

    auto log_services = get_manager<LogService>().get_entries(bmc.get_manager_uuid());

    if (!log_services.empty()) {
        if (was_sel_changed(ipmi)) {
            agent_framework::model::attribute::EventData::Vector events{};

            auto sel_records = get_records(ipmi);
            auto sel_service = log_services.front();
            LogEntry log_entry{sel_service.get_uuid()};
            log_entry.set_agent_id(bmc.get_id());

            const auto before_change_epoch = get_manager<LogEntry>().get_current_epoch();

            for(auto sel_record : sel_records) {
                auto record = std::static_pointer_cast<SelRecordGeneric>(sel_record);
                if (record->is_evm_1_5()) {
                    fill_entry(log_entry, record);
                    m_stabilizer.stabilize(log_entry);
                    add_or_update(events, log_entry);
                }
            }

            remove_untouched(bmc.get_id(), before_change_epoch, events);

            agent_framework::eventing::EventsQueue::get_instance()->push_back(events);
        }
    }
}


void SELReader::add_or_update(agent_framework::model::attribute::EventData::Vector& events, const LogEntry& log_entry){
    const auto status = get_manager<LogEntry>().add_or_update_entry(log_entry);

    auto add_event = [&events](const LogEntry& entry,
                               agent_framework::model::enums::Notification notification_type) {

        agent_framework::model::attribute::EventData event{};
        event.set_parent(entry.get_parent_uuid());
        event.set_component(entry.get_uuid());
        event.set_type(entry.get_component());
        event.set_notification(notification_type);
        events.push_back(std::move(event));
    };

    if (agent_framework::module::TableInterface::UpdateStatus::Added == status) {
        add_event(log_entry, agent_framework::model::enums::Notification::Add);
    }
    else if (agent_framework::module::TableInterface::UpdateStatus::StatusChanged == status ||
             agent_framework::module::TableInterface::UpdateStatus::Updated == status) {
        add_event(log_entry, agent_framework::model::enums::Notification::Update);
    }
}


void SELReader::remove_untouched(const std::string& bmc_id,
                                 const std::uint64_t before_change_epoch,
                                 agent_framework::model::attribute::EventData::Vector& events) {

    auto not_touched = get_manager<LogEntry>().get_keys(
            [&before_change_epoch, &bmc_id](const LogEntry& entry) {
                return (entry.get_agent_id() == bmc_id
                    && !entry.was_touched_after(before_change_epoch)
                    && entry.get_entry_type() == enums::EntryType::SEL);
            }
    );

    for (const auto& to_remove_uuid : not_touched) {
        agent_framework::model::attribute::EventData event{};

        get_manager<LogEntry>().remove_entry(to_remove_uuid,
            [&event](const LogEntry& elem) {
                event.set_parent(elem.get_parent_uuid());
                event.set_component(elem.get_uuid());
                event.set_type(elem.get_component());
                event.set_notification(agent_framework::model::enums::Notification::Remove);
            });
        if (!event.get_component().empty()) {
            events.push_back(std::move(event));
        }
    }
}


void SELReader::fill_entry(LogEntry& log_entry, const std::shared_ptr<SelRecordGeneric>& sel_record) {
    log_entry.set_entry_type(enums::EntryType::SEL);
    if (sel_record->get_timestamp() > SEL_INIT_TIME) {
        std::stringstream timestamp;
        timestamp << agent_framework::utils::make_iso_8601_timestamp(static_cast<time_t>(sel_record->get_timestamp()));
        log_entry.set_event_timestamp(timestamp.str());
    }
    log_entry.set_record_id(sel_record->get_id());
    sel_record->is_asserted() ? log_entry.set_entry_code(enums::EntryCode::Assert)
                              : log_entry.set_entry_code(enums::EntryCode::Deassert);
    log_entry.set_sensor_type(ipmi_sensor_to_redfish(sel_record->get_sensor_type()));
    log_entry.set_sensor_number(sel_record->get_sensor_number());

    std::stringstream msg;
    msg << to_hex_string<1, true>(sel_record->get_event_data1());
    msg << to_hex_string<1, false>(sel_record->get_event_data2() & SelRecordGeneric::EVENT_DATA_PLAIN_VALUE_MASK);
    msg << to_hex_string<1, false>(sel_record->get_event_data3() & SelRecordGeneric::EVENT_DATA_PLAIN_VALUE_MASK);
    log_entry.set_message_id(msg.str());
}


bool SELReader::was_sel_changed(IpmiController& ipmi) {
    command::generic::request::GetSelInfo req{};
    command::generic::response::GetSelInfo resp{};
    ipmi.send(req, resp);

    if (resp.get_last_add_timestamp() == m_last_add_timestamp &&
        resp.get_last_erase_timestamp() == m_last_remove_timestamp) {
        return false;
    }
    else {
        m_last_add_timestamp = resp.get_last_add_timestamp();
        m_last_remove_timestamp = resp.get_last_erase_timestamp();
        return true;
    }
}


Sel::RecordVect SELReader::get_records(IpmiController& ipmi) {
    ipmi::Sel sel(ipmi);
    sel.process_records();
    return sel.get_records();
}


OptionalField<SELReader::RedfishSensorType> SELReader::ipmi_sensor_to_redfish(IpmiSensorType sensor_type) {
    if (m_ipmi_redfish_sensor_map.find(sensor_type) == m_ipmi_redfish_sensor_map.end()) {
        return OptionalField<RedfishSensorType>{};
    }
    else {
        auto redfish_sensor = m_ipmi_redfish_sensor_map.at(sensor_type);
        return OptionalField<RedfishSensorType>{redfish_sensor};
    }
}
