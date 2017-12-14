/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.services.context;

import java.util.Collection;

import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toSet;

public enum ContextType {
    MANAGER,
    STORAGE_SERVICE,
    COMPOSED_NODE,
    PHYSICAL_DRIVE(STORAGE_SERVICE),
    LOGICAL_DRIVE(STORAGE_SERVICE),
    REMOTE_TARGET(STORAGE_SERVICE),

    CHASSIS,
    THERMAL(CHASSIS),
    POWER(CHASSIS),
    REDUNDANCY(THERMAL, POWER),
    POWER_VOLTAGE(POWER),
    POWER_CONTROL(POWER),
    POWER_SUPPLY(POWER),
    THERMAL_FAN(THERMAL),
    THERMAL_TEMPERATURE(THERMAL),
    COMPUTER_SYSTEM,
    SIMPLE_STORAGE(COMPUTER_SYSTEM),
    STORAGE(COMPUTER_SYSTEM),
    MEMORY(COMPUTER_SYSTEM),
    PROCESSOR(COMPUTER_SYSTEM),
    ETHERNET_INTERFACE(COMPUTER_SYSTEM, MANAGER),
    NETWORK_INTERFACE(COMPUTER_SYSTEM),
    NETWORK_DEVICE_FUNCTION(NETWORK_INTERFACE),
    COMPUTER_SYSTEM_METRICS(COMPUTER_SYSTEM),
    PROCESSOR_METRICS(PROCESSOR),
    MEMORY_METRICS(MEMORY),

    ETHERNET_SWITCH,
    ETHERNET_SWITCH_METRICS(ETHERNET_SWITCH),
    ETHERNET_SWITCH_ACL(ETHERNET_SWITCH),
    ETHERNET_SWITCH_ACL_RULE(ETHERNET_SWITCH_ACL),
    ETHERNET_SWITCH_PORT(ETHERNET_SWITCH),
    ETHERNET_SWITCH_PORT_METRICS(ETHERNET_SWITCH_PORT),
    ETHERNET_SWITCH_PORT_VLAN(ETHERNET_SWITCH_PORT, ETHERNET_INTERFACE),
    ETHERNET_SWITCH_STATIC_MAC(ETHERNET_SWITCH_PORT),
    NETWORK_PROTOCOL(MANAGER),

    DRIVE(CHASSIS),
    PCIE_DEVICE(CHASSIS),
    PCIE_DEVICE_FUNCTION(PCIE_DEVICE),
    FABRIC,

    ZONE(FABRIC),
    SWITCH(FABRIC),
    PORT(SWITCH),
    ENDPOINT(FABRIC),
    PORT_METRICS(PORT),

    EVENT_SERVICE,
    EVENT_SUBSCRIPTION(EVENT_SERVICE),

    TELEMETRY_SERVICE,
    METRIC_DEFINITION(TELEMETRY_SERVICE),
    METRIC_REPORT_DEFINITION(TELEMETRY_SERVICE);

    private final Collection<ContextType> possibleParents;

    ContextType(ContextType... possibleParents) {
        this.possibleParents = stream(possibleParents).collect(toSet());
    }

    Collection<ContextType> getPossibleParents() {
        return possibleParents;
    }
}
