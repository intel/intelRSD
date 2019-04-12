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
 *
 * @file chassis.hpp
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#pragma once

#include "enum_builder.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM ChassisType for Chassis class Type member
 */
ENUM(ChassisType, uint32_t, Rack, Drawer, Module, Blade, Enclosure, StandAlone, RackMount, Card, Cartridge, Row, Pod,
     Expansion, Sidecar, Zone, Sled, Shelf, Component, IPBasedDrive, Other);

/*!
 * @brief ENUM HashMethod for AuthorizationCertificate class
 */
ENUM(HashMethod, uint32_t, MD5, UNKNOWN);

/*!
 * @brief ENUM EncodingMethod for AuthorizationCertificate class
 * */
ENUM(EncodingMethod, uint32_t, BASE64, UNKNOWN);

/*!
 * @brief ENUM Platform type for Chassis class
 */
ENUM(PlatformType, uint32_t, BDCR, EDK, MF3, GRANTLEY, PURLEY, UNKNOWN);

/*!
 * @brief ENUM SerialConsoleSignalType for Manager class Serial Console subclass member
 */
ENUM(SerialConsoleSignalType, uint32_t, Rs232, Rs485);

/*!
 * @brief ENUM SerialConsoleParity for Manager class Serial Console subclass member
 */
ENUM(SerialConsoleParity, uint32_t, None, Even, Odd, Mark, Space);

/*!
 * @brief ENUM SerialConsoleFlowControl for Manager class Serial Console subclass member
 */
ENUM(SerialConsoleFlowControl, uint32_t, None, Software, Hardware);

/*!
 * @brief ENUM SerialConsolePinOut for Manager class Serial Console subclass member
 */
ENUM(SerialConsolePinOut, uint32_t, Cisco, Cyclades, Digi);

/*!
 * @brief ENUM PowerSupplyType for Psu subclass member
 */
ENUM(PowerSupplyType, uint32_t, AC, DC, ACorDC);

/*!
 * @brief ENUM LineInputVoltageType for Psu subclass member
 */
ENUM(LineInputVoltageType, uint32_t, Unknown, ACLowLine, ACMidLine, ACHighLine, DCNeg48V, DC380V, AC120V, AC240V,
     AC277V, ACandDCWideRange, ACWideRange, DC240V);

/*!
 * @brief ENUM SpeedUnits for Fan subclass member
 */
ENUM(SpeedUnits, uint32_t, Percent, RPM);

/*!
 * @brief ENUM PhysicalContext for Fan subclass member
 */
ENUM(PhysicalContext, uint32_t, Room, Intake, Exhaust, Front, Back, Upper, Lower, CPU, GPU, Backplane, SystemBoard,
     PowerSupply, VoltageRegulator, StorageDevice, NetworkingDevice, ComputeBay, StorageBay, NetworkBay, ExpansionBay,
     PowerSupplyBay);

/*!
 * @brief ENUM ReadingUnits for ChassisSensor subclass member
 */
ENUM(ReadingUnits, uint32_t, Volt, Celsius);

}
}
}
