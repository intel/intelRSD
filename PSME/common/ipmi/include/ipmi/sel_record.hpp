/*!
 * @brief System Event Log record (event)
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
 * @file ipmi/sel_record.hpp
 */

#pragma once

#include "ipmi/command/generic/sel/sel_defs.hpp"

#include <cstdint>
#include <vector>
#include <memory>

namespace ipmi {

class Sel;

class SelRecord {

    friend class Sel;

public:
    using RecordData = std::vector<std::uint8_t>;
    using Ptr = std::shared_ptr<SelRecord>;

    static ipmi::command::generic::SelEntryId get_id(const RecordData& record);
    static ipmi::command::generic::SelRecordType get_type(const RecordData& record);
    static ipmi::command::generic::SelTimestamp get_timestamp(const RecordData& record);
    static bool has_timestamp(const RecordData& record);
    static bool is_oem(const RecordData& record);


    /*! Default constructor */
    SelRecord(const RecordData& record);

    /*! Destructor */
    virtual ~SelRecord();

    /*!
     * @brief Get record id
     * @return record id
     */
    ipmi::command::generic::SelEntryId get_id() const {
        return id;
    }

    /*!
     * @brief Get record type
     * @return record type
     */
    ipmi::command::generic::SelRecordType get_type() const {
        return type;
    }

    /*!
     * @brief Check if record has timestamp
     * @return true if record is from record family with timestamps
     */
    bool has_timestamp() const;

    /*!
     * @brief Get record creation/modification timestamp
     * @return record timestamp
     */
    ipmi::command::generic::SelTimestamp get_timestamp() const {
        return timestamp;
    }

    /*!
     * @brief Get string representation of the message
     * @return dummy string with record id
     */
    virtual std::string to_string() const = 0;

private:
    const ipmi::command::generic::SelEntryId id;
    const ipmi::command::generic::SelRecordType type;
    const ipmi::command::generic::SelTimestamp timestamp;
    bool only_in_memory{false};
};

class SelRecordOem : public SelRecord {
public:
    /*! Default constructor */
    SelRecordOem(const RecordData& record);

    /*!
     * @brief Get string representation of the message
     * @return string with all properties and plain bytes
     */
    std::string to_string() const override;

private:
    const RecordData buffer;
};


class SelRecordGeneric: public SelRecord {
public:
    using SensorNumber = std::uint8_t;
    using EventType = std::uint8_t;
    using EventOffset = std::uint8_t;
    using EventData = std::uint16_t;

    /*!
     * @brief Event type identifier containing event "definition"
     *
     * There are "packed" all necessary fields.
     * @warning All bits are utilized, if necessary simple int must be replaced
     *          with the struct.
     */
    using EventSensorType = std::uint64_t;

    /*!
     * @brief Magic numbers for (additional and extended) event data
     * @{
     */
    /* SensorNumber */ static constexpr std::uint64_t EVENT_SENSOR_MASK = 0xff;
    /* EventType    */ static constexpr std::uint64_t EVENT_TYPE_MASK = 0x7f;
    /* EventOffset  */ static constexpr std::uint64_t EVENT_DATA_OFFSET_MASK = 0x0f;
    /* std::uint8_t */ static constexpr std::uint64_t SENSOR_TYPE_MASK = 0xff;
    /* EventOffset */  static constexpr std::uint64_t UNDEFINED_OFFSET = 0x0f;
    /* SensorNumber */ static constexpr std::uint64_t UNDEFINED_SENSOR_NUMBER = 0x00;

    /* EventData */    static constexpr std::uint64_t EVENT_DATA_AVAILABLE_MASK = 0x0300;
    /* EventData */    static constexpr std::uint64_t EVENT_DATA_AVAILABLE_OEM = 0x0200;
    /* EventData */    static constexpr std::uint64_t EVENT_DATA_PLAIN_VALUE_MASK = 0x00ff;
    /* EventData */    static constexpr std::uint64_t EVENT_DATA_VALUE_MASK = 0x03ff;
    /* EventData */    static constexpr std::uint64_t EVENT_DATA_NOT_PRESENT = 0x00ff;

    static constexpr unsigned DATA2_BIT_POS = 0;
    static constexpr unsigned DATA3_BIT_POS = 10;
    static constexpr unsigned OFFSET_BIT_POS = 20;
    static constexpr unsigned SENSOR_TYPE_BIT_POS = 24;
    static constexpr unsigned EVENT_TYPE_BIT_POS = 32;
    static constexpr unsigned SENSOR_NUMBER_BIT_POS = 40;
    static constexpr unsigned DATA2_MASK_POS = 48;
    static constexpr unsigned DATA3_MASK_POS = 56;
    /*! @} */

    static EventType get_event_type(const RecordData& record);

    /*! Get sensor type */
    ipmi::command::generic::SensorType get_sensor_type() const {
        return sensor_type;
    }
    /*! Get sensor number */
    SensorNumber get_sensor_number() const {
        return sensor_number;
    }
    /*! Check if event was asserted */
    bool is_asserted() const {
        return asserted;
    }
    /*! Get event type */
    EventType get_event_type() const {
        return event_type;
    }
    /*! Get sensor specific offset */
    EventOffset get_event_offset() const {
        return event_offset;
    }
    /*! Get event additional data #2 */
    EventData get_event_data2() const {
        return event_data2;
    }
    /*! Get event additional data #3 */
    EventData get_event_data3() const {
        return event_data3;
    }

    /*!
     * @brief Check if event is for given sensor type
     * @param sensor to be checked
     * @return true if event is emitted from sensor of given type
     */
    bool is_from(ipmi::command::generic::SensorType sensor) const {
        return get_sensor_type() == sensor;
    }

    /*!
     * @brief Get detailed event identifier
     * @param code event type code category
     * @param sensor sensor type id
     * @param offset event specific offset
     * @param data2 additional event data
     * @param data3 extended event data
     * @return detailed event type
     * @warning must be defined here, no overload allowed
     * @{
     */
    static constexpr EventSensorType event_sensor_type(EventType code,
                                                       ipmi::command::generic::SensorType sensor,
                                                       EventOffset offset,
                                                       EventData data2, EventData data3) {
        return static_cast<EventSensorType>(
            ((static_cast<EventSensorType>(code)   & EVENT_TYPE_MASK) << EVENT_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(sensor) & SENSOR_TYPE_MASK) << SENSOR_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(offset) & EVENT_DATA_OFFSET_MASK) << OFFSET_BIT_POS) |
            ((static_cast<EventSensorType>(data2)  & EVENT_DATA_VALUE_MASK) << DATA2_BIT_POS) |
            ((static_cast<EventSensorType>(data3)  & EVENT_DATA_VALUE_MASK) << DATA3_BIT_POS));
    }

    static constexpr EventSensorType event_sensor_type(EventType code,
                                                       ipmi::command::generic::SensorType sensor,
                                                       EventOffset offset,
                                                       EventData data2) {
        return static_cast<EventSensorType>(
            ((static_cast<EventSensorType>(code)   & EVENT_TYPE_MASK) << EVENT_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(sensor) & SENSOR_TYPE_MASK) << SENSOR_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(offset) & EVENT_DATA_OFFSET_MASK) << OFFSET_BIT_POS) |
            ((static_cast<EventSensorType>(data2)  & EVENT_DATA_VALUE_MASK) << DATA2_BIT_POS) |
            ((EVENT_DATA_NOT_PRESENT & EVENT_DATA_VALUE_MASK) << DATA3_BIT_POS));
    }

    static constexpr EventSensorType event_sensor_type(EventType code,
                                                       ipmi::command::generic::SensorType sensor,
                                                       EventOffset offset) {
        return static_cast<EventSensorType>(
            ((static_cast<EventSensorType>(code)   & EVENT_TYPE_MASK) << EVENT_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(sensor) & SENSOR_TYPE_MASK) << SENSOR_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(offset) & EVENT_DATA_OFFSET_MASK) << OFFSET_BIT_POS) |
            ((EVENT_DATA_NOT_PRESENT & EVENT_DATA_VALUE_MASK) << DATA2_BIT_POS) |
            ((EVENT_DATA_NOT_PRESENT & EVENT_DATA_VALUE_MASK) << DATA3_BIT_POS));
    }

    static constexpr EventSensorType event_sensor_type(EventType code,
                                                       ipmi::command::generic::SensorType sensor) {
        return static_cast<EventSensorType>(
            ((static_cast<EventSensorType>(code)   & EVENT_TYPE_MASK) << EVENT_TYPE_BIT_POS) |
            ((static_cast<EventSensorType>(sensor) & SENSOR_TYPE_MASK) << SENSOR_TYPE_BIT_POS) |
            ((UNDEFINED_OFFSET       & EVENT_DATA_OFFSET_MASK) << OFFSET_BIT_POS) |
            ((EVENT_DATA_NOT_PRESENT & EVENT_DATA_VALUE_MASK) << DATA2_BIT_POS) |
            ((EVENT_DATA_NOT_PRESENT & EVENT_DATA_VALUE_MASK) << DATA3_BIT_POS));
    }
    /*! @} */

    static constexpr EventData oem_data(EventData data) {
        return static_cast<EventData>(
            (((data & EVENT_DATA_PLAIN_VALUE_MASK) == EVENT_DATA_NOT_PRESENT) ? EVENT_DATA_NOT_PRESENT
             : ((data & EVENT_DATA_AVAILABLE_MASK) == 0) ? (EVENT_DATA_AVAILABLE_OEM | data)
             : data) & EVENT_DATA_VALUE_MASK);
    }

    /*!
     * @brief Set masked bits in data2
     * @param base already defined event "specification" (without additional event identification)
     * @param mask which bits must be cleared before comparision
     * @return EventSensorType
     */
    static constexpr EventSensorType mask_data2(EventSensorType base, std::uint8_t mask) {
        return static_cast<EventSensorType>(
            /* mask bits to be set: both data2 and data3 */
            (base & (~((EVENT_DATA_PLAIN_VALUE_MASK << DATA2_MASK_POS)))) |
            (((~mask) & EVENT_DATA_PLAIN_VALUE_MASK) << DATA2_MASK_POS));
    }
    /*!
     * @brief Set masked bits in data2
     * @param base already defined event "specification" (without additional event identification)
     * @param mask which bits must be cleared before comparision
     * @return EventSensorType
     */
    static constexpr EventSensorType mask_data3(EventSensorType base, std::uint8_t mask) {
        return static_cast<EventSensorType>(
            /* mask bits to be set: both data2 and data3 */
            (base & (~((EVENT_DATA_PLAIN_VALUE_MASK << DATA3_MASK_POS)))) |
            (((~mask) & EVENT_DATA_PLAIN_VALUE_MASK) << DATA3_MASK_POS));
    }

    /*!
     * @brief Get detailed event identifier
     * @param base already defined event "specification" (without additional event identification)
     * @param data2 additional event data
     * @return event identifier
     * @warning OEM data is assumed if not specified
     */
    static constexpr EventSensorType for_oem_data2(EventSensorType base, EventData data2) {
        return static_cast<EventSensorType>(
            (base & (~(EVENT_DATA_VALUE_MASK << DATA2_BIT_POS))) |
            (static_cast<EventSensorType>(oem_data(data2)) << DATA2_BIT_POS));
    }

    /*!
     * @brief Get detailed event identifier
     * @param base already defined event "specification" (without additional event identification)
     * @param data3 extended event data
     * @return event identifier
     * @warning OEM data is assumed if not specified
     */
    static constexpr EventSensorType for_oem_data3(EventSensorType base, EventData data3) {
        return static_cast<EventSensorType>(
            (base & (~(EVENT_DATA_VALUE_MASK << DATA3_BIT_POS))) |
            (static_cast<EventSensorType>(oem_data(data3)) << DATA3_BIT_POS));
    }


    /*!
     * @brief get detailed event identifier for given sensor
     * @param base already defined event "specification" (without sensor number)
     * @param sensor_number sensor number to be checked
     */
    static constexpr EventSensorType for_sensor(EventSensorType base, SensorNumber sensor_number) {
        return static_cast<EventSensorType>(
            (base & (~(EVENT_SENSOR_MASK << SENSOR_NUMBER_BIT_POS))) |
            ((static_cast<EventSensorType>(sensor_number) & EVENT_SENSOR_MASK) << SENSOR_NUMBER_BIT_POS));
    }

    /*!
     * @brief get detailed event identifier for given offset
     * @param base already defined event "specification" (without sensor number)
     * @param offset offset of the event
     */
    static constexpr EventSensorType for_offset(EventSensorType base, EventOffset offset) {
        return static_cast<EventSensorType>(
            (base & (~(EVENT_DATA_OFFSET_MASK << OFFSET_BIT_POS))) |
            ((static_cast<EventSensorType>(offset) & EVENT_DATA_OFFSET_MASK) << OFFSET_BIT_POS));
    }

    /*!
     * @brief append processor id
     * @param base "base" event definition
     * @param cpu id of the cpu [0..7]
     * @return Composite event id with CPU masking
     */
    static constexpr EventSensorType for_cpu(EventSensorType base, unsigned cpu) {
        return mask_data3(for_oem_data3(base, static_cast<EventData>((cpu & 0x07) << 5)), 0xe0);
    }

    /*!
     * @brief append memory identification
     * @param base event definition
     * @param instance instance of the DIMM (in whole system)
     * @return Composite event id with DIMM identification
     */
    static constexpr EventSensorType for_memory(EventSensorType base, unsigned instance) {
        return for_oem_data3(base, static_cast<EventData>(
            // bits 7-5: CPU
            ((instance / 6) << 5) |
            // bits 4-2: DIMM
            ((instance % 6) << 2)));
    }

    /*!
     * @brief Check if event matches given sensor event type.
     *
     * It is possible to check if event comes from "events family". Both exact (with additional data checking) and
     * "less exact" types are allowed.
     *
     * @param se_type sensor event type, valid enumeration in SelRecordSensor or constructed with event_sensor_type()
     * @return true if event matches
     */
    bool is_of_type(EventSensorType se_type) const;

    /*!
     * @return String representation of the event
     */
    std::string to_string() const override;

protected:
    /*! Default constructor */
    SelRecordGeneric(const RecordData& record);

    virtual std::string get_info() const = 0;

private:
    /*!
     * @brief check sensor specific data, if defined.
     *
     * There are two forms: if AVAILABLILITY is "unspecified" (in definition), only
     * plain value is checked. If AVAILABILITY bits are passed in definition, both must equal.
     *
     * @param definition appropriate bits from EventSensorType
     * @param checked appropriate event data from the event
     * @return true if event data "matches"
     */
    static bool event_data_match(EventData definition, EventData checked);

    /*!
     * @brief Method to validate the record prior the fields.
     * @warning Used in the constructor with valid_record field to be called as first one
     * @param record record to be analized
     * @throws runtime_error (with proper description) if something is wrong
     * @return unused value
     */
    static bool is_valid(const RecordData& record);
    const bool valid_record;

    const ipmi::command::generic::SensorType sensor_type;
    const SensorNumber sensor_number;
    const bool asserted;
    const EventType event_type;
    const EventOffset event_offset;

    static EventData get_event_data(const RecordData& record, unsigned pos);
    const EventData event_data2;
    const EventData event_data3;
};


class SelRecordThreshold : public SelRecordGeneric {
public:
    SelRecordThreshold(const RecordData& record) : SelRecordGeneric(record) { }

protected:
    std::string get_info() const override;
};


class SelRecordDiscrete : public SelRecordGeneric {
public:
    SelRecordDiscrete(const RecordData& record) : SelRecordGeneric(record) { }

protected:
    std::string get_info() const override;
};


class SelRecordSensor : public SelRecordGeneric {
public:
    static constexpr EventType EVENT_TYPE_CODE = 0x6f;

    SelRecordSensor(const RecordData& record) : SelRecordGeneric(record) { }

    /*!
     * @brief sensor event detailed types
     *
     * These defs are taken from IPMI spec.
     * Macros used for "expansion" (because of --pedantic) don't work, additional "no args" must
     * be added either
     * @{
     */
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    #endif

    #define SS(t)      event_sensor_type(EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t)
    #define SE(t, ...) event_sensor_type(EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t, ##__VA_ARGS__)

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif

    static constexpr EventSensorType TEMPERATURE = SS(TEMPERATURE);
    static constexpr EventSensorType VOLTAGE = SS(VOLTAGE);
    static constexpr EventSensorType CURRENT = SS(CURRENT);
    static constexpr EventSensorType FAN = SS(FAN);

    /* Physical Security */
    static constexpr EventSensorType GENERAL_CHASSIS_INTRUSION = SE(PHYSICAL_SECURITY, 0x00);
    static constexpr EventSensorType DRIVE_BAY_INTRUSION = SE(PHYSICAL_SECURITY, 0x01);
    static constexpr EventSensorType IO_CARD_AREA_INTRUSION = SE(PHYSICAL_SECURITY, 0x02);
    static constexpr EventSensorType PROCESSOR_AREA_INTRUSION = SE(PHYSICAL_SECURITY, 0x03);
    static constexpr EventSensorType LAN_LEASH_LOST = SE(PHYSICAL_SECURITY, 0x04);
    static constexpr EventSensorType UNAUTHORIZED_DOCK = SE(PHYSICAL_SECURITY, 0x05);
    static constexpr EventSensorType FAN_AREA_INTRUSION = SE(PHYSICAL_SECURITY, 0x06);

    /* Platform Security Violation Attempt */
    static constexpr EventSensorType SECURE_MODE_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x00);
    static constexpr EventSensorType USER_PASSWORD_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x01);
    static constexpr EventSensorType SETUP_PASSWORD_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x02);
    static constexpr EventSensorType NETWORK_PASSWORD_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x03);
    static constexpr EventSensorType OTHER_PASSWORD_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x04);
    static constexpr EventSensorType OOB_PASSWORD_VIOLATION = SE(PERFORM_SECURITY_VIOLATION, 0x05);

    /* Processor */
    static constexpr EventSensorType IERR = SE(PROCESSOR, 0x00);
    static constexpr EventSensorType THERMAL_TRIP = SE(PROCESSOR, 0x01);
    static constexpr EventSensorType FRB1_BIST_FAILURE = SE(PROCESSOR, 0x02);
    static constexpr EventSensorType FRB2_HANG_IN_POST = SE(PROCESSOR, 0x03);
    static constexpr EventSensorType FRB3_INITIALIZATION_FAILURE = SE(PROCESSOR, 0x04);
    static constexpr EventSensorType CONFIGURATION_ERROR = SE(PROCESSOR, 0x05);
    static constexpr EventSensorType SMBIOS_UNCORRECTABLE_ERROR = SE(PROCESSOR, 0x06);
    static constexpr EventSensorType PROCESSOR_PRESENCE_DETECTED = SE(PROCESSOR, 0x07);
    static constexpr EventSensorType PROCESSOR_DISABLED = SE(PROCESSOR, 0x08);
    static constexpr EventSensorType TERMINATOR_PRESENCE_DETECTED = SE(PROCESSOR, 0x09);
    static constexpr EventSensorType PROCESSOR_AUTOMATIC_THROTTLED = SE(PROCESSOR, 0x0a);
    static constexpr EventSensorType MACHINE_CHECK_EXCEPTION = SE(PROCESSOR, 0x0b);
    static constexpr EventSensorType MACHINE_CHECK_ERROR = SE(PROCESSOR, 0x0c);

    /* Power Supply */
    static constexpr EventSensorType PS_PRESENCE_DETECTED = SE(POWER_SUPPLY, 0x00);
    static constexpr EventSensorType PS_FAILURE = SE(POWER_SUPPLY, 0x01);
    static constexpr EventSensorType PS_PREDICTIVE_FAILURE = SE(POWER_SUPPLY, 0x02);
    static constexpr EventSensorType PS_INPUT_LOST = SE(POWER_SUPPLY, 0x03);
    static constexpr EventSensorType PS_INPUT_LOST_OR_OUT_OF_RANGE = SE(POWER_SUPPLY, 0x04);
    static constexpr EventSensorType PS_INPUT_OUT_OF_RANGE = SE(POWER_SUPPLY, 0x05);
    static constexpr EventSensorType PS_CONFIGURATION_ERROR = SE(POWER_SUPPLY, 0x06);
    static constexpr EventSensorType PS_INACTIVE = SE(POWER_SUPPLY, 0x07);

    /* Power unit */
    static constexpr EventSensorType POWER_OFF = SE(POWER_UNIT, 0x00);
    static constexpr EventSensorType POWER_CYCLE = SE(POWER_UNIT, 0x01);
    static constexpr EventSensorType POWER_DOWN = SE(POWER_UNIT, 0x02);
    static constexpr EventSensorType POWER_INTERLOCK_DOWN = SE(POWER_UNIT, 0x03);
    static constexpr EventSensorType POWER_INPUT_LOST = SE(POWER_UNIT, 0x04);
    static constexpr EventSensorType SOFT_CONTROL_FAILURE = SE(POWER_UNIT, 0x05);
    static constexpr EventSensorType POWER_UNIT_FAILURE = SE(POWER_UNIT, 0x06);
    static constexpr EventSensorType POWER_UNIT_PREDICTIVE_FAILURE = SE(POWER_UNIT, 0x07);

    /* Memory */
    static constexpr EventSensorType CORRECTABLE_ECC = SE(MEMORY, 0x00);
    static constexpr EventSensorType UNCORRECTABLE_ECC = SE(MEMORY, 0x01);
    static constexpr EventSensorType PARITY_ERROR = SE(MEMORY, 0x02);
    static constexpr EventSensorType MEMORY_SCRUB_FAILED = SE(MEMORY, 0x03);
    static constexpr EventSensorType MEMORY_DEVICE_DISABLED = SE(MEMORY, 0x04);
    static constexpr EventSensorType LOGGING_LIMIT_REACHED = SE(MEMORY, 0x05);
    static constexpr EventSensorType MEMORY_PRESENCE_DETECTED = SE(MEMORY, 0x06);
    static constexpr EventSensorType MEMORY_CONFIGURATION_ERROR = SE(MEMORY, 0x07);
    static constexpr EventSensorType MEMORY_SPARE = SE(MEMORY, 0x08);
    static constexpr EventSensorType MEMORY_AUTOMATIC_THROTTLED = SE(MEMORY, 0x09);
    static constexpr EventSensorType CRITICAL_OVERTEMPERATURE = SE(MEMORY, 0x0a);

    /* Drive slot */
    static constexpr EventSensorType DRIVE_PRESENCE = SE(DRIVE_SLOT, 0x00);
    static constexpr EventSensorType DRIVE_FAULT = SE(DRIVE_SLOT, 0x01);
    static constexpr EventSensorType DRIVE_PREDICTIVE_FAILURE = SE(DRIVE_SLOT, 0x02);
    static constexpr EventSensorType DRIVE_HOT_SPARE = SE(DRIVE_SLOT, 0x03);
    static constexpr EventSensorType DRIVE_CONSISTENCY_CHECK = SE(DRIVE_SLOT, 0x04);
    static constexpr EventSensorType DRIVE_IN_CRITICAL_ARRAY = SE(DRIVE_SLOT, 0x05);
    static constexpr EventSensorType DRIVE_IN_FAILE_ARRAY = SE(DRIVE_SLOT, 0x06);
    static constexpr EventSensorType DRIVE_REMAP_IN_PROGRESS = SE(DRIVE_SLOT, 0x07);
    static constexpr EventSensorType DRIVE_REMAP_ABORTED = SE(DRIVE_SLOT, 0x08);

    /* System Firmware Progress, POST errors */
    static constexpr EventSensorType SYSTEM_FIRMWARE_ERROR = SE(POST_ERRORS, 0x00);

    static constexpr EventSensorType NO_SYSTEM_MEMORY_INSTALLED = SE(POST_ERRORS, 0x00, 0x01);
    static constexpr EventSensorType ALL_MEMORY_FAILED = SE(POST_ERRORS, 0x00, 0x02);
    static constexpr EventSensorType UNRECOVERABLE_HDD_FAILURE = SE(POST_ERRORS, 0x00, 0x03);
    static constexpr EventSensorType UNRECOVERABLE_BOARD_FAILURE = SE(POST_ERRORS, 0x00, 0x04);
    static constexpr EventSensorType UNRECOVERABLE_DISKETTE_FAILURE = SE(POST_ERRORS, 0x00, 0x05);
    static constexpr EventSensorType UNRECOVERABLE_HDD_CTRL_FAILURE = SE(POST_ERRORS, 0x00, 0x06);
    static constexpr EventSensorType UNRECOVERABLE_USB_FAILURE = SE(POST_ERRORS, 0x00, 0x07);
    static constexpr EventSensorType REMOVABLE_BOOT_MEDIA_NOT_FOUND = SE(POST_ERRORS, 0x00, 0x08);
    static constexpr EventSensorType UNRECOVERABLE_VIDEO_CONTROLLER_FAILURE = SE(POST_ERRORS, 0x00, 0x09);
    static constexpr EventSensorType NO_VIDEO_DEVICE_DETECTED = SE(POST_ERRORS, 0x00, 0x0a);
    static constexpr EventSensorType ROM_CORRUPTION_DETECTED = SE(POST_ERRORS, 0x00, 0x0b);
    static constexpr EventSensorType CPU_VOLTAGE_MISMATCH = SE(POST_ERRORS, 0x00, 0x0c);
    static constexpr EventSensorType CPU_SPEED_MATCHING_FAILURE = SE(POST_ERRORS, 0x00, 0x0d);

    static constexpr EventSensorType SYSTEM_FIRMWARE_HANG = SE(POST_ERRORS, 0x01);

    static constexpr EventSensorType SYSTEM_FIRMWARE_PROGRESS = SE(POST_ERRORS, 0x02);

    #undef SE
    #undef SS
    /*! @} */

protected:
    std::string get_info() const override;
};


class SelRecordGenericOEM : public SelRecordGeneric {
public:
    SelRecordGenericOEM(const RecordData& record) : SelRecordGeneric(record) { }

protected:
    std::string get_info() const override;
};


class SelRecordUnspecified : public SelRecord {
public:
    SelRecordUnspecified(const RecordData& record) : SelRecord(record) { }

protected:
    std::string to_string() const override;
};

}
