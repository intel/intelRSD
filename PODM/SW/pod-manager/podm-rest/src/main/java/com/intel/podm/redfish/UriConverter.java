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

package com.intel.podm.redfish;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.types.Id;

import java.net.URI;
import java.util.Arrays;
import java.util.Map;
import java.util.Optional;
import java.util.function.Function;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.google.common.collect.Maps.newHashMap;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.ADAPTER;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.LOGICAL_DRIVE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.common.types.Id.fromString;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.regex.Pattern.compile;

public final class UriConverter {
    private static final Map<ContextType, Function> CONVERTERS = newHashMap();

    static {
        registerConverter(ContextType.CHASSIS, UriConverter::getChassisContextFromUri);
        registerConverter(ContextType.PROCESSOR, UriConverter::getProcessorContextFromUri);
        registerConverter(ContextType.MEMORY, UriConverter::getMemoryContextFromUri);
        registerConverter(ContextType.LOGICAL_DRIVE, UriConverter::getLogicalDriveContextFromUri);
        registerConverter(ContextType.DEVICE, UriConverter::getLocalDriveContextFromUri);
        registerConverter(ContextType.SIMPLE_STORAGE, UriConverter::getSimpleStorageContextFromUri);
        registerConverter(ContextType.ETHERNET_INTERFACE, UriConverter::getEthernetInterfaceContextFromUri);
        registerConverter(ContextType.ETHERNET_SWITCH_PORT, UriConverter::getEthernetSwitchPortContextFromUri);
        registerConverter(ContextType.ETHERNET_SWITCH_PORT_VLAN, UriConverter::getEthernetSwitchPortVlanContextFromUri);
    }

    private static final Pattern CHASSIS_URI_PATTERN = compile(
            "/redfish/v1/Chassis/(?<chassisId>\\d+)"
    );

    private static final Pattern PROCESSOR_URI_PATTERN = compile(
            "/redfish/v1/Systems/(?<systemId>\\d+)/Processors/(?<processorId>\\d+)"
    );

    private static final Pattern MEMORY_URI_PATTERN = compile(
            "/redfish/v1/Systems/(?<systemId>\\d+)/Memory/(?<memoryId>\\d+)"
    );

    private static final Pattern LOGICAL_DRIVE_URI_PATTERN = compile(
            "/redfish/v1/Services/(?<serviceId>\\d+)/LogicalDrives/(?<driveId>\\d+)"
    );

    private static final Pattern LOCAL_DRIVE_URI_PATTERN = compile(
            "/redfish/v1/Systems/(?<systemId>\\d+)/Adapters/(?<adapterId>\\d+)/Devices/(?<deviceId>\\d+)"
    );

    private static final Pattern SIMPLE_STORAGE_URI_PATTERN = compile(
            "/redfish/v1/Systems/(?<systemId>\\d+)/SimpleStorage/(?<simpleStorageId>\\d+)"
    );

    private static final Pattern ETHERNET_INTERFACE_URI_PATTERN = compile(
            "/redfish/v1/Systems/(?<systemId>\\d+)/EthernetInterfaces/(?<ethernetInterfaceId>\\d+)"
    );

    private static final Pattern ETHERNET_SWITCH_PORT_URI_PATTERN = compile(
            "/redfish/v1/EthernetSwitches/(?<ethernetSwitchId>\\d+)/Ports/(?<portId>\\d+)"
    );

    private static final Pattern ETHERNET_SWITCH_PORT_VLAN_URI_PATTERN = compile(
            "/redfish/v1/EthernetSwitches/(?<ethernetSwitchId>\\d+)/Ports/(?<portId>\\d+)/VLANs/(?<vlanId>\\d+)"
    );

    private UriConverter() {
    }

    public static Context getContextFromUri(URI uri, ContextType... contextTypes) {
        if (contextTypes == null || contextTypes.length == 0) {
            throw new UriConversionException(format("Cannot parse URI [%s] to unknown type", uri));
        }

        Optional<Context> context;
        for (ContextType contextType : contextTypes) {
            if (!CONVERTERS.containsKey(contextType)) {
                continue;
            }

            context = (Optional<Context>) CONVERTERS.get(contextType).apply(uri);
            if (context.isPresent()) {
                return context.get();
            }
        }

        throw new UriConversionException(format("Cannot parse URI [%s] to any type from %s", uri, Arrays.toString(contextTypes)));
    }

    private static Optional<Context> getChassisContextFromUri(URI uri) {
        Matcher matcher = CHASSIS_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id chassisId = fromString(matcher.group("chassisId"));

        return of(contextOf(chassisId, CHASSIS));
    }

    private static Optional<Context> getProcessorContextFromUri(URI uri) {
        Matcher matcher = PROCESSOR_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id systemId = fromString(matcher.group("systemId"));
        Id processorId = fromString(matcher.group("processorId"));

        return of(contextOf(systemId, COMPUTER_SYSTEM)
                .child(processorId, PROCESSOR));
    }

    private static Optional<Context> getMemoryContextFromUri(URI uri) {
        Matcher matcher = MEMORY_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id systemId = fromString(matcher.group("systemId"));
        Id memoryId = fromString(matcher.group("memoryId"));

        return of(contextOf(systemId, COMPUTER_SYSTEM)
                .child(memoryId, MEMORY));
    }

    private static Optional<Context> getLogicalDriveContextFromUri(URI uri) {
        Matcher matcher = LOGICAL_DRIVE_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id serviceId = fromString(matcher.group("serviceId"));
        Id driveId = fromString(matcher.group("driveId"));

        return of(contextOf(serviceId, STORAGE_SERVICE)
                .child(driveId, LOGICAL_DRIVE));
    }

    private static Optional<Context> getLocalDriveContextFromUri(URI uri) {
        Matcher matcher = LOCAL_DRIVE_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id systemId = fromString(matcher.group("systemId"));
        Id adapterId = fromString(matcher.group("adapterId"));
        Id deviceId = fromString(matcher.group("deviceId"));

        return of(contextOf(systemId, COMPUTER_SYSTEM)
                .child(adapterId, ADAPTER)
                .child(deviceId, DEVICE));
    }

    private static Optional<Context> getSimpleStorageContextFromUri(URI uri) {
        Matcher matcher = SIMPLE_STORAGE_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id systemId = fromString(matcher.group("systemId"));
        Id simpleStorageId = fromString(matcher.group("simpleStorageId"));

        return of(contextOf(systemId, COMPUTER_SYSTEM)
                .child(simpleStorageId, SIMPLE_STORAGE));
    }

    private static Optional<Context> getEthernetInterfaceContextFromUri(URI uri) {
        Matcher matcher = ETHERNET_INTERFACE_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id systemId = fromString(matcher.group("systemId"));
        Id ethernetInterfaceId = fromString(matcher.group("ethernetInterfaceId"));

        return of(contextOf(systemId, COMPUTER_SYSTEM)
                .child(ethernetInterfaceId, ETHERNET_INTERFACE));
    }

    private static Optional<Context> getEthernetSwitchPortContextFromUri(URI uri) {
        Matcher matcher = ETHERNET_SWITCH_PORT_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id ethernetSwitchId = fromString(matcher.group("ethernetSwitchId"));
        Id portId = fromString(matcher.group("portId"));

        return of(contextOf(ethernetSwitchId, ETHERNET_SWITCH)
                .child(portId, ETHERNET_SWITCH_PORT));
    }

    private static Optional<Context> getEthernetSwitchPortVlanContextFromUri(URI uri) {
        Matcher matcher = ETHERNET_SWITCH_PORT_VLAN_URI_PATTERN.matcher(uri.toString());
        if (!matcher.matches()) {
            return empty();
        }

        Id ethernetSwitchId = fromString(matcher.group("ethernetSwitchId"));
        Id portId = fromString(matcher.group("portId"));
        Id vlanId = fromString(matcher.group("vlanId"));

        return of(contextOf(ethernetSwitchId, ETHERNET_SWITCH)
                .child(portId, ETHERNET_SWITCH_PORT)
                .child(vlanId, ETHERNET_SWITCH_PORT_VLAN));
    }

    private static void registerConverter(ContextType contextType, Function<URI, Optional<Context>> converter) {
        CONVERTERS.put(contextType, converter);
    }
}
