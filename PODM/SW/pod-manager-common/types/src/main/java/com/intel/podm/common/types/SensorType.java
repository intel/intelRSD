/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.common.types;

public enum SensorType implements EnumeratedType {
    PLATFORM_SECURITY_VIOLATION_ATTEMPT("Platform Security Violation Attempt"),
    TEMPERATURE("Temperature"),
    VOLTAGE("Voltage"),
    CURRENT("Current"),
    FAN("Fan"),
    PHYSICAL_CHASSIS_SECURITY("Physical Chassis Security"),
    PROCESSOR("Processor"),
    POWER_SUPPLY_CONVERTER("Power Supply / Converter"),
    POWER_UNIT("PowerUnit"),
    COOLLING_DEVICE("CoolingDevice"),
    OTHER_UNITS_BASED_SENSOR("Other Units-based Sensor"),
    MEMORY("Memory"),
    DRIVE_SLOT_BAY("Drive Slot/Bay"),
    POST_MEMORY_RESIZE("POST Memory Resize"),
    SYSTEM_FIRMWARE_PROGRESS("System Firmware Progress"),
    EVENT_LOGGING_DISABLED("Event Logging Disabled"),
    SYSTEM_EVENT("System Event"),
    CRITICAL_INTERRUPT("Critical Interrupt"),
    BUTTON_SWITCH("Button/Switch"),
    MODULE_BOARD("Module/Board"),
    MICROCONTROLLER_COPROCESSOR("Microcontroller/Coprocessor"),
    ADD_IN_CARD("Add-in Card"),
    CHASSIS("Chassis"),
    CHIP_SET("ChipSet"),
    OTHER_FRU("Other FRU"),
    CABLE_INTERCONNECT("Cable/Interconnect"),
    TERMINATOR("Terminator"),
    SYSTEM_BOOT_RESTART("SystemBoot/Restart"),
    BOOT_ERROR("Boot Error"),
    BASE_OS_BOOT_INSTALLATION_STATUS("BaseOSBoot/InstallationStatus"),
    OS_STOP_SHUTDOWN("OS Stop/Shutdown"),
    SLOT_CONNECTOR("Slot/Connector"),
    SYSTEM_ACPI_POWER_STATE("System ACPI PowerState"),
    WATCHDOG("Watchdog"),
    PLATFORM_ALLERT("Platform Alert"),
    ENTITY_PRESENCE("Entity Presence"),
    MONITOR_ASIC_IC("Monitor ASIC/IC"),
    LAN("LAN"),
    MANAGEMENT_SUBSYSTEM_HEALTH("Management Subsystem Health"),
    BATTERY("Battery"),
    SESSION_AUDIT("Session Audit"),
    VERSION_CHANGE("Version Change"),
    FRU_STATE("FRUState");

    private final String value;

    SensorType(String value) {
        this.value = value;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
