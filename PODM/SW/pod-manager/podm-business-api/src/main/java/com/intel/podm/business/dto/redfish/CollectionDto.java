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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.common.types.Id;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class CollectionDto {

    private final Type type;
    private final List<Id> members;

    public CollectionDto(Type type, Collection<Id> members) {
        this.type = type;
        this.members = Collections.unmodifiableList(new ArrayList<>(members));
    }

    public Type getType() {
        return  type;
    }

    public List<Id> getMembers() {
        return members;
    }

    public enum Type {
        CHASSIS,
        PROCESSORS,
        MANAGERS,
        ETHERNET_SWITCHES,
        ETHERNET_SWITCH_PORTS,
        ETHERNET_SWITCH_PORT_VLANS,
        SERVICES,
        SYSTEMS,
        SUBSCRIPTIONS,
        ETHERNET_INTERFACES,
        SIMPLE_STORAGE,
        MEMORY_MODULES,
        MEMORY_CHUNKS,
        COMPOSED_NODES,
        ADAPTERS,
        DEVICES,
        REMOTE_TARGETS,
        LOGICAL_DRIVES,
        PHYSICAL_DRIVES,
        THERMAL_ZONES,
        POWER_ZONES
    }
}
