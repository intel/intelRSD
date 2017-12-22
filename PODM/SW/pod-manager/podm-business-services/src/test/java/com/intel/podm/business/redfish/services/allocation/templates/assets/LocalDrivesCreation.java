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

package com.intel.podm.business.redfish.services.allocation.templates.assets;

import com.intel.podm.business.entities.redfish.SimpleStorageDevice;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.LocalDrive;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;

import java.math.BigDecimal;

import static java.math.BigDecimal.valueOf;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@SuppressWarnings({"checkstyle:ParameterNumber"})
public final class LocalDrivesCreation {
    private LocalDrivesCreation() {
    }

    public static RequestedNode.LocalDrive createRequestedDrive(MediaType type, Integer capacityGib,
                                                                Protocol protocol,
                                                                BigDecimal minRpm, String serialNumber, Context context) {
        RequestedNode.LocalDrive mock = mock(RequestedNode.LocalDrive.class);
        when(mock.getCapacityGib()).thenReturn(capacityGib == null ? null : valueOf(capacityGib));
        when(mock.getType()).thenReturn(type);
        when(mock.getMinRpm()).thenReturn(minRpm);
        when(mock.getSerialNumber()).thenReturn(serialNumber);
        when(mock.getInterface()).thenReturn(protocol);
        when(mock.getResourceContext()).thenReturn(context);

        return mock;
    }

    public static RequestedNode.LocalDrive createRequestedDriveWithChassisContext(MediaType type, Integer capacityGib,
                                                                Protocol protocol,
                                                                BigDecimal minRpm, String serialNumber, Context resourceContext, Context chassisContext) {
        RequestedNode.LocalDrive mock = LocalDrivesCreation.createRequestedDrive(type, capacityGib, protocol, minRpm, serialNumber, resourceContext);
        when(mock.getChassisContext()).thenReturn(chassisContext);

        return mock;
    }

    public static SimpleStorageDevice createAvailableSimpleStorage(BigDecimal capacityGib, Id id) {
        return createAvailableStorage(SimpleStorageDevice.class, null, capacityGib, null, null, null, id);
    }

    public static LocalDrive createAvailableLocalDrive(MediaType type, BigDecimal capacityGib, Protocol protocol,
                                                       BigDecimal rpm, String serialNumber, Id id) {
        return createAvailableStorage(LocalDrive.class, type, capacityGib, protocol, rpm, serialNumber, id);
    }

    private static <T extends LocalStorage> T createAvailableStorage(Class<T> storageClass, MediaType type, BigDecimal capacityGib,
                                                                     Protocol protocol, BigDecimal rpm, String serialNumber, Id id) {
        T mock = mock(storageClass);
        when(mock.getCapacityGib()).thenReturn(capacityGib);
        when(mock.getType()).thenReturn(type);
        when(mock.getRpm()).thenReturn(rpm);
        when(mock.getSerialNumber()).thenReturn(serialNumber);
        when(mock.getProtocol()).thenReturn(protocol);
        when(mock.getId()).thenReturn(id);

        return mock;
    }
}
