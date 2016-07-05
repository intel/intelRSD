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

package com.intel.podm.templates.assets;

import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.entities.redfish.properties.SimpleStorageDevice;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.StorageControllerInterface;

import java.math.BigDecimal;

import static java.math.BigDecimal.valueOf;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public final class LocalDrivesCreation {
    private LocalDrivesCreation() {
    }

    public static RequestedLocalDrive createRequestedDrive(DriveType type, Integer capacityGib,
                                                           StorageControllerInterface iface,
                                                           Integer minRpm, String serialNumber, Context context) {
        RequestedLocalDrive mock = mock(RequestedLocalDrive.class);
        when(mock.getCapacityGib()).thenReturn(capacityGib == null ? null : valueOf(capacityGib));
        when(mock.getType()).thenReturn(type);
        when(mock.getMinRpm()).thenReturn(minRpm);
        when(mock.getSerialNumber()).thenReturn(serialNumber);
        when(mock.getInterface()).thenReturn(iface);
        when(mock.getResourceContext()).thenReturn(context);

        return mock;
    }

    public static SimpleStorageDevice createAvailableSimpleStorage(BigDecimal capacityGib, Id id) {
        return createAvailableStorage(SimpleStorageDevice.class, null, capacityGib, null, null, null, id);
    }

    public static Device createAvailableLocalDrive(DriveType type, BigDecimal capacityGib, StorageControllerInterface iface,
                                                   Integer rpm, String serialNumber, Id id) {
        return createAvailableStorage(Device.class, type, capacityGib, iface, rpm, serialNumber, id);
    }

    private static <T extends LocalStorage> T createAvailableStorage(Class<T> storageClass, DriveType type, BigDecimal capacityGib,
                                                                     StorageControllerInterface iface, Integer rpm, String serialNumber, Id id) {
        T mock = mock(storageClass);
        when(mock.getCapacityGib()).thenReturn(capacityGib);
        when(mock.getType()).thenReturn(type);
        when(mock.getRpm()).thenReturn(rpm);
        when(mock.getSerialNumber()).thenReturn(serialNumber);
        when(mock.getInterface()).thenReturn(iface);
        when(mock.getId()).thenReturn(id);

        return mock;
    }
}
