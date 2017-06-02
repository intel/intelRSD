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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.common.types.Id;

import java.util.Collections;
import java.util.Set;

public class CollectionDto {

    private final Type type;
    private final Set<Id> members;

    public CollectionDto(Type type, Set<Id> members) {
        this.type = type;
        this.members = Collections.unmodifiableSet(members);
    }

    public Type getType() {
        return type;
    }

    public Set<Id> getMembers() {
        return members;
    }

    public enum Type {
        CHASSIS,
        DRIVES,
        PROCESSORS,
        MANAGERS,
        ENDPOINTS,
        ETHERNET_SWITCHES,
        ETHERNET_SWITCH_PORTS,
        ETHERNET_SWITCH_PORT_VLANS,
        SERVICES,
        SYSTEMS,
        EVENT_SUBSCRIPTIONS,
        ETHERNET_INTERFACES,
        NETWORK_INTERFACES,
        NETWORK_DEVICE_FUNTIONS,
        SIMPLE_STORAGE,
        STORAGE,
        MEMORY_MODULES,
        COMPOSED_NODES,
        REMOTE_TARGETS,
        LOGICAL_DRIVES,
        PHYSICAL_DRIVES,
        THERMAL_ZONES,
        POWER_ZONES,
        PCIE_DEVICES,
        PCIE_DEVICE_FUNCTIONS,
        FABRICS,
        ZONES,
        FABRIC_SWITCHES,
        FABRIC_SWITCH_PORTS
    }
}
