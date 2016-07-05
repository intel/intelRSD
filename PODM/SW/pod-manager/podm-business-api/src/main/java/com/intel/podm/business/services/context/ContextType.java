/*
 * Copyright (c) 2015 Intel Corporation
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
    THERMAL_ZONE(CHASSIS),
    POWER_ZONE(CHASSIS),
    COMPUTER_SYSTEM,
    SIMPLE_STORAGE(COMPUTER_SYSTEM),
    MEMORY(COMPUTER_SYSTEM),
    PROCESSOR(COMPUTER_SYSTEM),
    MEMORY_CHUNK(COMPUTER_SYSTEM),
    ETHERNET_INTERFACE(COMPUTER_SYSTEM, MANAGER),

    ETHERNET_SWITCH,
    ETHERNET_SWITCH_PORT(ETHERNET_SWITCH),
    ETHERNET_SWITCH_PORT_VLAN(ETHERNET_SWITCH_PORT, ETHERNET_INTERFACE),
    NETWORK_PROTOCOL(MANAGER),
    ADAPTER(COMPUTER_SYSTEM),
    DEVICE(ADAPTER);

    private final Collection<ContextType> possibleParents;

    ContextType(ContextType... possibleParents) {
        this.possibleParents = stream(possibleParents).collect(toSet());
    }

    Collection<ContextType> getPossibleParents() {
        return possibleParents;
    }
}
