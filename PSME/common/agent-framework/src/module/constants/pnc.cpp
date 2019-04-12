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
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file pnc.cpp
 * @brief Contains constants used by PNC agents
 * */

#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::literals;

constexpr const char Switch::SWITCH[];
constexpr const char Switch::STATUS[];
constexpr const char Switch::SWITCH_ID[];
constexpr const char Switch::CHASSIS[];
constexpr const char Switch::ASSET_TAG[];
constexpr const char Switch::FRU_INFO[];
constexpr const char Switch::SKU[];
constexpr const char Switch::COLLECTIONS[];
constexpr const char Switch::INDICATOR_LED[];
constexpr const char Switch::OEM[];
constexpr const char Switch::POWER_STATE[];
constexpr const char Switch::ALLOWED_ACTIONS[];
constexpr const char Switch::PROTOCOL[];

constexpr const char Fabric::FABRIC[];
constexpr const char Fabric::STATUS[];
constexpr const char Fabric::PROTOCOL[];
constexpr const char Fabric::COLLECTIONS[];
constexpr const char Fabric::OEM[];

constexpr const char Port::PORT[];
constexpr const char Port::STATUS[];
constexpr const char Port::PORT_ID[];
constexpr const char Port::PORT_TYPE[];
constexpr const char Port::CABLE_IDS[];
constexpr const char Port::SPEED_GBPS[];
constexpr const char Port::WIDTH[];
constexpr const char Port::MAX_SPEED_GBPS[];
constexpr const char Port::MAX_WIDTH[];
constexpr const char Port::OEM[];
constexpr const char Port::ALLOWED_ACTIONS[];
constexpr const char Port::POWER_STATE[];
constexpr const char Port::PROTOCOL[];

constexpr const char PcieDevice::DEVICE[];
constexpr const char PcieDevice::STATUS[];
constexpr const char PcieDevice::DEVICE_ID[];
constexpr const char PcieDevice::DEVICE_CLASS[];
constexpr const char PcieDevice::SPEED_GBPS[];
constexpr const char PcieDevice::WIDTH[];
constexpr const char PcieDevice::MAX_SPEED_GBPS[];
constexpr const char PcieDevice::MAX_WIDTH[];
constexpr const char PcieDevice::FIRMWARE_VERSION[];
constexpr const char PcieDevice::CHASSIS[];
constexpr const char PcieDevice::ASSET_TAG[];
constexpr const char PcieDevice::FRU_INFO[];
constexpr const char PcieDevice::SKU[];
constexpr const char PcieDevice::COLLECTIONS[];
constexpr const char PcieDevice::OEM[];

constexpr const char PcieFunction::FUNCTION[];
constexpr const char PcieFunction::STATUS[];
constexpr const char PcieFunction::FUNCTION_ID[];
constexpr const char PcieFunction::FUNCTION_TYPE[];
constexpr const char PcieFunction::DEVICE_CLASS[];
constexpr const char PcieFunction::PCI_DEVICE_ID[];
constexpr const char PcieFunction::PCI_VENDOR_ID[];
constexpr const char PcieFunction::PCI_CLASS_CODE[];
constexpr const char PcieFunction::PCI_REVISION_ID[];
constexpr const char PcieFunction::PCI_SUBSYSTEM_ID[];
constexpr const char PcieFunction::PCI_SUBSYSTEM_VENDOR_ID[];
constexpr const char PcieFunction::FUNCTIONAL_DEVICE[];
constexpr const char PcieFunction::OEM[];
