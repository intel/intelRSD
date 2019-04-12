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
 * @file discovery_exceptions.hpp
 *
 * @brief Exceptions thrown during the pnc discovery
 * */

#pragma once

#include "agent-framework/module/pnc_components.hpp"

#include <exception>

namespace agent {
namespace pnc {
namespace discovery {

/*! Thrown when no PCIe switch is found in the OS - no need for further discovery */
class PncDiscoveryExceptionSwitchNotFound final : public std::exception {
public:
    PncDiscoveryExceptionSwitchNotFound() = default;
    PncDiscoveryExceptionSwitchNotFound(const PncDiscoveryExceptionSwitchNotFound&) = default;
    ~PncDiscoveryExceptionSwitchNotFound();
};

/*! Thrown when no configuration data for port are provided */
class PncDiscoveryExceptionNoConfiguration final : public std::exception {
public:
    PncDiscoveryExceptionNoConfiguration() = default;
    PncDiscoveryExceptionNoConfiguration(const PncDiscoveryExceptionNoConfiguration&) = default;
    ~PncDiscoveryExceptionNoConfiguration();
};

/*! Thrown when no device data can be read - possibly the device is not of specified type */
class PncDiscoveryExceptionDeviceNotFound final : public std::exception {
public:

    PncDiscoveryExceptionDeviceNotFound() = default;
    PncDiscoveryExceptionDeviceNotFound(const PncDiscoveryExceptionDeviceNotFound&) = default;
    ~PncDiscoveryExceptionDeviceNotFound();
};


}
}
}
