/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "psme/rest/constants/constants.hpp"



namespace psme {
namespace rest {
namespace constants {

namespace Chassis {
const char CHASSIS_TYPE[] = "ChassisType";
const char INDICATOR_LED[] = "IndicatorLED";
const char LOCATION[] = "Location";
const char PARENT_ID[] = "ParentId";
const char CONTAINS[] = "Contains";
const char COMPUTER_SYSTEMS[] = "ComputerSystems";
const char ETHERNET_SWITCHES[] = "EthernetSwitches";
const char MANAGERS_IN_CHASSIS[] = "ManagersInChassis";
const char POWERED_BY[] = "PoweredBy";
const char COOLED_BY[] = "CooledBy";
const char THERMAL[] = "Thermal";
const char POWER[] = "Power";
const char DRIVES[] = "Drives";
const char STORAGE[] = "Storage";
const char PCIE_DEVICES[] = "PCIeDevices";
const char GEO_TAG[] = "GeoTag";
const char RACK_SUPPORTS_DISAGGREGATED_POWER_COOLING[] = "RackSupportsDisaggregatedPowerCooling";
const char POWER_STATE[] = "PowerState";
const char HASH_CHASSIS_RESET[] = "#Chassis.Reset";
const char CHASSIS_RESET[] = "Chassis.Reset";
}

namespace PowerZone {
const char VOLTAGES[] = "Voltages";
const char POWER_CONTROL[] = "PowerControl";
const char POWER_AVAILABLE_WATTS[] = "PowerAvailableWatts";
const char POWER_CAPACITY_WATTS[] = "PowerCapacityWatts";
const char POWER_CONSUMED_WATTS[] = "PowerConsumedWatts";
const char POWER_CONSUMED_WATTS_PATH[] = "/PowerConsumedWatts";
const char POWER_REQUESTED_WATTS[] = "PowerRequestedWatts";
const char POWER_ALLOCATED_WATTS[] = "PowerAllocatedWatts";
const char POWER_SUPPLIES[] = "PowerSupplies";
const char POWER_METRICS[] = "PowerMetrics";
const char POWER_LIMIT[] = "PowerLimit";
const char INTERVAL_IN_MIN[] = "IntervalInMin";
const char MIN_CONSUMED_WATTS[] = "MinConsumedWatts";
const char MAX_CONSUMED_WATTS[] = "MaxConsumedWatts";
const char AVERAGE_CONSUMED_WATTS[] = "AverageConsumedWatts";
const char LIMIT_IN_WATTS[] = "LimitInWatts";
const char LIMIT_EXCEPTION[] = "LimitException";
const char CORRECTION_IN_MS[] = "CorrectionInMs";
const char POWER_SUPPLY_TYPE[] = "PowerSupplyType";
const char LINE_INPUT_VOLTAGE_TYPE[] = "LineInputVoltageType";
const char LINE_INPUT_VOLTAGE[] = "LineInputVoltage";
const char LAST_POWER_OUTPUT_WATTS[] = "LastPowerOutputWatts";
const char SPARE_PART_NUMBER[] = "SparePartNumber";
const char INPUT_RANGES[] = "InputRanges";
const char INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE[] = "Intel.Oem.RequestPowerSupplyStateChange";
const char HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE[] = "#Intel.Oem.RequestPowerSupplyStateChange";
const char STATE_ALLOWABLE_VALUES[] = "State@AllowableValues";
const char MEMBER_ID_ALLOWABLE_VALUES[] = "MemberId@AllowableValues";
const char INPUT_AC_POWER_WATTS[] = "InputACPowerWatts";
const char INPUT_AC_POWER_WATTS_PATH[] = "/Oem/Intel_RackScale/InputACPowerWatts";
}

namespace ThermalZone {
const char FANS[] = "Fans";
const char READING[] = "Reading";
const char READING_PATH[] = "/Reading";
const char READING_UNITS[] = "ReadingUnits";
const char TEMPERATURES[] = "Temperatures";
const char DESIRED_SPEED_PWM[] = "DesiredSpeedPwm";
const char VOLUMETRIC_AIRFLOW_CFM[] = "VolumetricAirflowCfm";
const char VOLUMETRIC_AIRFLOW_CFM_PATH[] = "/Oem/Intel_RackScale/VolumetricAirflowCfm";
}

namespace ChassisSensor {
const char PHYSICAL_CONTEXT[] = "PhysicalContext";
const char UPPER_THRESHOLD_NON_CRITICAL[] = "UpperThresholdNonCritical";
const char UPPER_THRESHOLD_CRITICAL[] = "UpperThresholdCritical";
const char UPPER_THRESHOLD_FATAL[] = "UpperThresholdFatal";
const char LOWER_THRESHOLD_NON_CRITICAL[] = "LowerThresholdNonCritical";
const char LOWER_THRESHOLD_CRITICAL[] = "LowerThresholdCritical";
const char LOWER_THRESHOLD_FATAL[] = "LowerThresholdFatal";
const char MIN_READING_RANGE[] = "MinReadingRange";
const char MAX_READING_RANGE[] = "MaxReadingRange";
const char MIN_READING_RANGE_TEMP[] = "MinReadingRangeTemp";
const char MAX_READING_RANGE_TEMP[] = "MaxReadingRangeTemp";
const char SENSOR_NUMBER[] = "SensorNumber";
const char READING_CELSIUS[] = "ReadingCelsius";
const char READING_CELSIUS_PATH[] = "/ReadingCelsius";
const char READING_VOLTS[] = "ReadingVolts";
const char READING_VOLTS_PATH[] = "/ReadingVolts";
}

namespace Drive {
const char SECURE_ERASE[] = "#Drive.SecureErase";
const char SECURE_ERASE_ENDPOINT[] = "Drive.SecureErase";
const char CAPABLE_SPEED[] = "CapableSpeedGbs";
const char INDICATOR_LED[] = "IndicatorLED";
const char ENDPOINTS[] = "Endpoints";
const char VOLUMES[] = "Volumes";
const char MEDIA_TYPE[] = "MediaType";
const char NEGOTIATED_SPEED[] = "NegotiatedSpeedGbs";
const char PROTOCOL[] = "Protocol";
const char CAPACITY_BYTES[] = "CapacityBytes";
const char INFO[] = "Info";
const char INFO_FORMAT[] = "InfoFormat";
const char ERASE_ON_DETACH[] = "EraseOnDetach";
const char DRIVE_ERASED[] = "DriveErased";
const char FIRMWARE_VERSION[] = "FirmwareVersion";
const char LATENCY_TRACKING_ENABLED[] = "LatencyTrackingEnabled";
const char STATUS_INDICATOR[] = "StatusIndicator";
const char REVISION[] = "Revision";
const char FAILURE_PREDICTED[] = "FailurePredicted";
const char HOTSPARE_TYPE[] = "HotspareType";
const char ENCRYPTION_ABILITY[] = "EncryptionAbility";
const char ENCRYPTION_STATUS[] = "EncryptionStatus";
const char ROTATION_SPEED_RPM[] = "RotationSpeedRPM";
const char BLOCK_SIZE_BYTES[] = "BlockSizeBytes";
const char PREDICTED_MEDIA_LIFE_LEFT[] = "PredictedMediaLifeLeftPercent";
const char PCIE_FUNCTION[] = "PCIeFunction";
const char USED_BY[] = "UsedBy";
const char INTERFACE[] = "Interface";
const char CAPACITY[] = "CapacityGiB";
const char TYPE[] = "Type";
const char RPM[] = "RPM";
const char BUS_INFO[] = "BusInfo";
}

namespace PcieDevice {
const char PCIE_FUNCTIONS[] = "PCIeFunctions";
const char DEVICE_TYPE[] = "DeviceType";
}

namespace PcieFunction {
const char FUNCTION_ID[] = "FunctionId";
const char FUNCTION_TYPE[] = "FunctionType";
const char DEVICE_CLASS[] = "DeviceClass";
const char DEVICE_ID[] = "DeviceId";
const char VENDOR_ID[] = "VendorId";
const char CLASS_CODE[] = "ClassCode";
const char REVISION_ID[] = "RevisionId";
const char SUBSYSTEM_ID[] = "SubsystemId";
const char SUBSYSTEM_VENDOR_ID[] = "SubsystemVendorId";
const char PCIE_DEVICE[] = "PCIeDevice";
}

namespace DriveMetrics {
const char TEMPERATURE_KELVIN[] = "TemperatureKelvin";
const char LIFETIME[] = "LifeTime";
const char HEALTH_DATA[] = "HealthData";

const char UNIT_SIZE_BYTES[] = "UnitSizeBytes";
const char UNITS_READ[] = "UnitsRead";
const char UNITS_WRITTEN[] = "UnitsWritten";
const char HOST_READ_COMMANDS[] = "HostReadCommands";
const char HOST_WRITE_COMMANDS[] = "HostWriteCommands";
const char POWER_CYCLES[] = "PowerCycles";
const char POWER_ON_HOURS[] = "PowerOnHours";
const char CONTROLLER_BUSY_TIME_MINUTES[] = "ControllerBusyTimeMinutes";
const char AVAILABLE_SPARE_PERCENTAGE[] = "AvailableSparePercentage";
const char PREDICTED_MEDIA_LIFE_USED_PERCENT[] = "PredictedMediaLifeUsedPercent";
const char UNSAFE_SHUTDOWNS[] = "UnsafeShutdowns";
const char MEDIA_ERRORS[] = "MediaErrors";

}

}
}
}
