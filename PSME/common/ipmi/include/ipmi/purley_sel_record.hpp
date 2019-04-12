/*!
 * @brief Events defined for Purley platform
 *
 * This is only definition class, not to be instantiated.
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
 * @file ipmi/purley_sel_record.hpp
 */

#pragma once

#include "ipmi/sel_record.hpp"

namespace ipmi {


class PurleySelRecord : public SelRecordGeneric {
public:
    static constexpr EventType TRIGGER_EVENT_TYPE_CODE = 0x03;
    static constexpr EventType SENSOR_EVENT_TYPE_CODE = 0x6f;
    static constexpr EventType OEM_70_EVENT_TYPE_CODE = 0x70;
    static constexpr EventType OEM_72_EVENT_TYPE_CODE = 0x72;
    static constexpr EventType OEM_76_EVENT_TYPE_CODE = 0x76;

    /*!
     * @brief Events defined in F08A_BMC_Sensor_Event_Spec_rev001.pdf
     */
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    #endif

    #define SS(e, t)      event_sensor_type(e##_EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t)
    #define SE(e, t, ...) event_sensor_type(e##_EVENT_TYPE_CODE, ipmi::command::generic::SensorType::t, ##__VA_ARGS__)

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif

    /* 1.2 Discrete sensors */
    static constexpr EventSensorType CPU_DIMM_HOT = for_sensor(SS(SENSOR, OEM_C6), 0x79);
    static constexpr EventSensorType CPU0_DIMM_HOT     = for_offset(CPU_DIMM_HOT, 0);
    static constexpr EventSensorType CPU1_DIMM_HOT     = for_offset(CPU_DIMM_HOT, 1);
    static constexpr EventSensorType CPU0_DIMM_ABC_HOT = for_offset(CPU_DIMM_HOT, 2);
    static constexpr EventSensorType CPU0_DIMM_DEF_HOT = for_offset(CPU_DIMM_HOT, 3);
    static constexpr EventSensorType CPU1_DIMM_GHJ_HOT = for_offset(CPU_DIMM_HOT, 4);
    static constexpr EventSensorType CPU1_DIMM_KLM_HOT = for_offset(CPU_DIMM_HOT, 5);

    static constexpr EventSensorType CPU0_ERROR = for_sensor(SS(SENSOR, OEM_C2), 0xA8);
    static constexpr EventSensorType CPU0_FIVR_FAULT   = for_offset(CPU0_ERROR, 0);
    static constexpr EventSensorType CPU0_THERMAL_TRIP = for_offset(CPU0_ERROR, 3);

    static constexpr EventSensorType CPU1_ERROR = for_sensor(SS(SENSOR, OEM_C2), 0xA9);
    static constexpr EventSensorType CPU1_FIVR_FAULT   = for_offset(CPU1_ERROR, 0);
    static constexpr EventSensorType CPU1_THERMAL_TRIP = for_offset(CPU1_ERROR, 3);

    // HSC_STS_HIGH
    // HSC_STS_LOW
    // EVENT_LOG

    static constexpr EventSensorType WATCHDOG = for_sensor(SS(SENSOR, WATCHDOG2), 0xE2);
    static constexpr EventSensorType WATCHDOG_TIMER_EXPIRED = for_offset(WATCHDOG, 0);
    static constexpr EventSensorType WATCHDOG_BIOS_FRB2 = for_oem_data2(WATCHDOG_TIMER_EXPIRED, 0x301);
    static constexpr EventSensorType WATCHDOG_BIOS_POST = for_oem_data2(WATCHDOG_TIMER_EXPIRED, 0x302);
    static constexpr EventSensorType WATCHDOG_OS_LOAD   = for_oem_data2(WATCHDOG_TIMER_EXPIRED, 0x303);
    static constexpr EventSensorType WATCHDOG_SMS_OS    = for_oem_data2(WATCHDOG_TIMER_EXPIRED, 0x304);
    static constexpr EventSensorType WATCHDOG_OEM       = for_oem_data2(WATCHDOG_TIMER_EXPIRED, 0x305);

    static constexpr EventSensorType SYS_BOOTING_STS = for_sensor(SS(SENSOR, OEM_CA), 0x78);
    static constexpr EventSensorType SYS_BOOTING_SLP_S4        = for_offset(SYS_BOOTING_STS, 0);
    static constexpr EventSensorType SYS_BOOTING_SLP_S3        = for_offset(SYS_BOOTING_STS, 1);
    static constexpr EventSensorType SYS_BOOTING_SLP_PCH_PWROK = for_offset(SYS_BOOTING_STS, 2);
    static constexpr EventSensorType SYS_BOOTING_SLP_SYS_PWROK = for_offset(SYS_BOOTING_STS, 3);
    static constexpr EventSensorType SYS_BOOTING_SLP_RESET     = for_offset(SYS_BOOTING_STS, 4);
    static constexpr EventSensorType SYS_BOOTING_SLP_BIOS_COMP = for_offset(SYS_BOOTING_STS, 5);

    static constexpr EventSensorType SYSTEM_STS = for_sensor(SS(OEM_70, CHASSIS), 0x77);
    static constexpr EventSensorType SYSTEM_STS_CPU0_OCCUPIED     = for_offset(SYSTEM_STS, 0);
    static constexpr EventSensorType SYSTEM_STS_CPU1_OCCUPIED     = for_offset(SYSTEM_STS, 1);
    static constexpr EventSensorType SYSTEM_STS_CPU0_THERMAL_TRIP = for_offset(SYSTEM_STS, 2);
    static constexpr EventSensorType SYSTEM_STS_CPU1_THERMAL_TRIP = for_offset(SYSTEM_STS, 3);
    static constexpr EventSensorType SYSTEM_STS_CPU0_FIVR_FAULT   = for_offset(SYSTEM_STS, 4);
    static constexpr EventSensorType SYSTEM_STS_CPU1_FIVR_FAULT   = for_offset(SYSTEM_STS, 5);
    static constexpr EventSensorType SYSTEM_STS_CPU_CATERR        = for_offset(SYSTEM_STS, 6);
    static constexpr EventSensorType SYSTEM_THROTTLE              = for_offset(SYSTEM_STS, 7);

    static constexpr EventSensorType VR_HOT = for_sensor(SS(SENSOR, OEM_C5), 0x7A);
    static constexpr EventSensorType VR_HOT_CPU0_VCCIN    = for_offset(VR_HOT, 0);
    static constexpr EventSensorType VR_HOT_CPU1_VCCIN    = for_offset(VR_HOT, 1);
    static constexpr EventSensorType VR_HOT_CPU0_DIMM_ABC = for_offset(VR_HOT, 2);
    static constexpr EventSensorType VR_HOT_CPU0_DIMM_DEF = for_offset(VR_HOT, 3);
    static constexpr EventSensorType VR_HOT_CPU1_DIMM_GHJ = for_offset(VR_HOT, 4);
    static constexpr EventSensorType VR_HOT_CPU1_DIMM_KLM = for_offset(VR_HOT, 5);

    static constexpr EventSensorType PROCESSOR_FAIL = for_sensor(SE(SENSOR, PROCESSOR, 4), 0x65);

    /* 1.3 Event only sensors */
    static constexpr EventSensorType CHASSIS_PWR_STS = for_sensor(SS(SENSOR, OEM_C8), 0x70);
    static constexpr EventSensorType CHASSIS_PWR_DOWN       = for_offset(CHASSIS_PWR_STS, 0);
    static constexpr EventSensorType CHASSIS_PWR_CYCLE      = for_offset(CHASSIS_PWR_STS, 1);
    static constexpr EventSensorType CHASSIS_PWR_ON         = for_offset(CHASSIS_PWR_STS, 2);
    static constexpr EventSensorType CHASSIS_PWR_SHUTDOWN   = for_offset(CHASSIS_PWR_STS, 3);
    static constexpr EventSensorType CHASSIS_PWR_AC_LOST    = for_offset(CHASSIS_PWR_STS, 4);
    static constexpr EventSensorType CHASSIS_PWR_HARD_RESET = for_offset(CHASSIS_PWR_STS, 5);

    // BUTTON

    static constexpr EventSensorType CATERR = for_sensor(SS(SENSOR, PROCESSOR), 0xE6);
    static constexpr EventSensorType CAT_IERR  = for_offset(CATERR, 0);
    static constexpr EventSensorType CAT_MCERR = for_offset(CATERR, 0x0b);

    static constexpr EventSensorType MSMI = for_sensor(SS(SENSOR, PROCESSOR), 0xE7);
    static constexpr EventSensorType MSMI_IERR  = for_offset(MSMI, 0);
    static constexpr EventSensorType MSMI_MCERR = for_offset(MSMI, 0x0b);

    // CRITICAL_IRQ:DIAGNOSTIC_INTERRUPT

    static constexpr EventSensorType MACHINE_CHECK_ERROR = for_sensor(SS(SENSOR, PROCESSOR), 0x40);
    static constexpr EventSensorType UNCORRECTABLE_MACHINE_CHECK_ERROR = for_offset(MACHINE_CHECK_ERROR, 0x0b);
    static constexpr EventSensorType CORRECTABLE_MACHINE_CHECK_ERROR = for_offset(MACHINE_CHECK_ERROR, 0x0c);
    static constexpr EventSensorType CPU0_UNCORRECTABLE_MACHINE_CHECK_ERROR = for_cpu(UNCORRECTABLE_MACHINE_CHECK_ERROR, 0);
    static constexpr EventSensorType CPU1_UNCORRECTABLE_MACHINE_CHECK_ERROR = for_cpu(UNCORRECTABLE_MACHINE_CHECK_ERROR, 1);
    static constexpr EventSensorType CPU0_CORRECTABLE_MACHINE_CHECK_ERROR = for_cpu(CORRECTABLE_MACHINE_CHECK_ERROR, 0);
    static constexpr EventSensorType CPU1_CORRECTABLE_MACHINE_CHECK_ERROR = for_cpu(CORRECTABLE_MACHINE_CHECK_ERROR, 1);

    // ME_STATUS
    // ADR_COMPLETE

    static constexpr EventSensorType MEMORY_ECC_ERROR = for_sensor(SS(SENSOR, MEMORY), 0x63);
    static constexpr EventSensorType MEMORY_ECC_ERROR_UNCORRECTABLE   = for_offset(MEMORY_ECC_ERROR, 0);
    static constexpr EventSensorType MEMORY_ECC_ERROR_PARITY          = for_offset(MEMORY_ECC_ERROR, 2);
    static constexpr EventSensorType MEMORY_ECC_ERROR_LOG_LIMIT       = for_offset(MEMORY_ECC_ERROR, 5);

    // DDR4_PPR
    // OTHER_IIO_ERROR

    static constexpr EventSensorType PCH_THERMAL_TRIP = for_sensor(SS(TRIGGER, TEMPERATURE), 0xED);

    // PCIE_ERROR

    static constexpr EventSensorType POST_ERROR = for_sensor(SS(SENSOR, POST_ERRORS), 0x2B);

    static constexpr EventSensorType POWER_ERROR = for_sensor(SS(SENSOR, OEM_C3), 0x74);
    static constexpr EventSensorType POWER_ERROR_SYS_PWROK_FAIL = for_offset(POWER_ERROR, 1);
    static constexpr EventSensorType POWER_ERROR_PCH_PWROK_FAIL = for_offset(POWER_ERROR, 2);
    static constexpr EventSensorType POWER_ERROR_SYS_VR_FAIL    = for_offset(POWER_ERROR, 3);

    // POWER_POLICY
    // POWER_STATE
    // SOFTWARE_NMI
    // SYSTEM_EVENT

    static constexpr EventSensorType CPU0_THERM_STATUS = for_sensor(SS(OEM_76, PROCESSOR), 0x1C);
    static constexpr EventSensorType CPU0_THERM_STATUS_CRITICAL   = for_offset(CPU0_THERM_STATUS, 0);
    static constexpr EventSensorType CPU0_THERM_STATUS_PROCHOT    = for_offset(CPU0_THERM_STATUS, 1);
    static constexpr EventSensorType CPU0_THERM_STATUS_TCC_ACTIVE = for_offset(CPU0_THERM_STATUS, 2);

    static constexpr EventSensorType CPU1_THERM_STATUS = for_sensor(SS(OEM_76, PROCESSOR), 0x1D);
    static constexpr EventSensorType CPU1_THERM_STATUS_CRITICAL   = for_offset(CPU1_THERM_STATUS, 0);
    static constexpr EventSensorType CPU1_THERM_STATUS_PROCHOT    = for_offset(CPU1_THERM_STATUS, 1);
    static constexpr EventSensorType CPU1_THERM_STATUS_TCC_ACTIVE = for_offset(CPU1_THERM_STATUS, 2);

    static constexpr EventSensorType NM_EXCEPTION = for_sensor(SS(OEM_72, OEM_DC), 0x18);
    static constexpr EventSensorType NM_EXCEPTION_PLATFORM      = for_oem_data2(NM_EXCEPTION, 0);
    static constexpr EventSensorType NM_EXCEPTION_CPU           = for_oem_data2(NM_EXCEPTION, 1);
    static constexpr EventSensorType NM_EXCEPTION_MEMORY        = for_oem_data2(NM_EXCEPTION, 2);
    static constexpr EventSensorType NM_EXCEPTION_HW_PROTECTION = for_oem_data2(NM_EXCEPTION, 3);
    static constexpr EventSensorType NM_EXCEPTION_IO_SUBSYSTEM  = for_oem_data2(NM_EXCEPTION, 4);

    // PWR_THRESH_EVNT

    #undef SE
    #undef SS
    /*! @} */
};


}
