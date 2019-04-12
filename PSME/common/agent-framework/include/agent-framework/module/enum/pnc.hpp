/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http_//www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file pnc.hpp
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once

#include "enum_builder.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM PciePortType for PciePort class
 *
 */
ENUM(PciePortType, uint32_t, UpstreamPort, DownstreamPort, InterswitchPort,
     ManagementPort, BidirectionalPort, UnconfiguredPort);

/*!
 * @brief ENUM PcieDeviceClass for PcieDevice class
 *
 */
ENUM(PcieDeviceClass, uint32_t, SingleFunction, MultiFunction, Simulated);

/*!
 * @brief ENUM PcieFunctionType for PcieFunction class
 *
 */
ENUM(PcieFunctionType, uint32_t, Physical, Virtual);

/*!
 * @brief ENUM PcieFunctionDeviceClass for PcieFunction class
 *
 */
ENUM(PcieFunctionDeviceClass, uint32_t, UnclassifiedDevice, MassStorageController,
    NetworkController, DisplayController, MultimediaController, MemoryController,
    Bridge, CommunicationController, GenericSystemPeripheral, InputDeviceController,
    DockingStation, Processor, SerialBusController, WirelessController,
    IntelligentController, SatelliteCommunicationsController, EncryptionController,
    SignalProcessingController, ProcessingAccelerators, NonEssentialInstrumentation,
    Coprocessor, UnassignedClass);

/*!
 * @brief ENUM PowerState for PcieSwitches
 */
ENUM(PcieSwitchPowerState, uint32_t, On, Off, PoweringOn, PoweringOff);

}
}
}
