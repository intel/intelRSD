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

package com.intel.podm.business.entities.base;

import com.tinkerpop.blueprints.Direction;

import static com.tinkerpop.blueprints.Direction.IN;
import static com.tinkerpop.blueprints.Direction.OUT;

public enum DomainObjectLink implements Link {

    /** AssetLinks */
    CONTAINS("contains", OUT), // Physically contains "out" asset
    CONTAINED_BY("contains", IN), // Is physically contained by "in" asset
    MEMBER("member", OUT), // Some collection has "out" asset as member
    MANAGED_BY("manages", IN), // Some asset is managed by "in" manager
    MANAGES("manages", OUT), // Some manager manages "out" asset
    MANAGER_IN_CHASSIS("managerLocation", OUT), // Some manager manages "out" asset
    MANAGERS_IN_CHASSIS("managerLocation", IN),

    /** BladeDeviceLink */
    PCI_DEVICES("pciDevices", OUT),
    USB_DEVICES("usbDevices", OUT),

    /** CollectionLink - groups domain objects in a node */
    COLLECTS("collects", OUT),
    COLLECTED_BY("collects", IN),

    /** ComposedNodeLink - logically includes (groups) assets in a node */
    INCLUDES("includes", OUT),
    INCLUDED("includes", IN),

    /** MemoryRegionsLink */
    REGIONS("regions", OUT),

    /** DiscoverableLink */
    OWNS("owns", OUT), //External service may own an "out" asset
    OWNED_BY("owns", IN), // Asset might be owned by external service

    /** IpAddressLink */
    IP_ADDRESS("ipAddress", OUT),

    /** ManagerConsoleLink */
    GRAPHIC_CONSOLE("graphicConsole", OUT),
    SERIAL_CONSOLE("serialConsole", OUT),
    COMMAND_SHELL("commandShell", OUT),

    /** NetworkInterfaceLink */
    IPV6_STATIC_ADDRESS("staticAddressIpV6", OUT),
    IPV6_ADDRESS_POLICY("policyIpV6", OUT),
    NAME_SERVER("nameServerLink", OUT),

    /** PortLink */
    PORT_MEMBERS("members", OUT),
    MEMBER_OF_PORT("members", IN),
    PRIMARY_VLAN("pvid", OUT),

    /** SimpleStorageDevices */
    SIMPLE_STORAGE_DEVICES("simpleStorageDevices", OUT),
    SIMPLE_STORAGE("simpleStorageDevices", IN),

    /** StorageLink */
    USES("uses", OUT),
    USED_BY("uses", IN),
    MASTERED_BY("masters", IN),

    /** Chassis */
    COMPUTER_SYSTEM("computerSystemLink", OUT),
    CHASSIS("computerSystemLink", IN);

    private final String label;
    private final Direction direction;

    DomainObjectLink(String label, Direction direction) {
        this.label = label;
        this.direction = direction;
    }

    @Override
    public String getLabel() {
        return label;
    }

    @Override
    public Direction getDirection() {
        return direction;
    }
}
