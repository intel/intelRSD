/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.services.context.Context;

import java.util.ArrayList;
import java.util.List;

import static java.util.Collections.unmodifiableList;

public class CollectionDto {
    private final Type type;
    private final List<Context> members;

    public CollectionDto(Type type, List<Context> members) {
        this.type = type;
        this.members = unmodifiableList(new ArrayList<>(members));
    }

    public Type getType() {
        return type;
    }

    public List<Context> getMembers() {
        return members;
    }

    public enum Type {
        CHASSIS,
        DRIVE,
        VOLUME,
        PROCESSOR,
        MANAGER,
        ENDPOINT,
        ETHERNET_SWITCH,
        ETHERNET_SWITCH_ACLS,
        ETHERNET_SWITCH_ACL_RULE,
        ETHERNET_SWITCH_PORT,
        ETHERNET_SWITCH_PORT_VLAN,
        ETHERNET_SWITCH_STATIC_MAC,
        SERVICE,
        SYSTEM,
        EVENT_SUBSCRIPTION,
        ETHERNET_INTERFACE,
        NETWORK_INTERFACE,
        NETWORK_DEVICE_FUNCTION,
        SIMPLE_STORAGE,
        STORAGE,
        MEMORY_MODULE,
        COMPOSED_NODE,
        THERMAL_ZONE,
        POWER_ZONE,
        PCIE_DEVICE,
        PCIE_DEVICE_FUNCTION,
        FABRIC,
        ZONE,
        FABRIC_SWITCH,
        FABRIC_SWITCH_PORT,
        METRIC_DEFINITION,
        METRIC_REPORT_DEFINITION,
        STORAGE_POOL
    }
}
