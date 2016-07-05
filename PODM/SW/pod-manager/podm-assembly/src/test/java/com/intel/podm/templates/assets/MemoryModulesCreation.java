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

import com.intel.podm.business.dto.redfish.RequestedMemory;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.Id;

import java.math.BigDecimal;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public final class MemoryModulesCreation {
    private MemoryModulesCreation() {
    }

    public static RequestedMemory createRequestedMemory(MemoryDeviceType memoryDeviceType, Integer achievableSpeedMhz, Integer dataWidthBits,
                                                        Integer sizeMib, String manufacturer) {
        return createRequestedMemory(memoryDeviceType, achievableSpeedMhz, dataWidthBits, sizeMib, manufacturer, null, null);
    }

    public static RequestedMemory createRequestedMemory(MemoryDeviceType memoryDeviceType, Integer achievableSpeedMhz, Integer dataWidthBits,
                                                        Integer sizeMib, String manufacturer, Context resourceContext) {
        return createRequestedMemory(memoryDeviceType, achievableSpeedMhz, dataWidthBits, sizeMib, manufacturer, resourceContext, null);
    }

    public static RequestedMemory createRequestedMemory(MemoryDeviceType memoryDeviceType, Integer achievableSpeedMhz, Integer dataWidthBits,
                                                        Integer sizeMib, String manufacturer, Context resourceContext, Context chassisContext) {
        RequestedMemory mock = mock(RequestedMemory.class);
        when(mock.getCapacityMib()).thenReturn(sizeMib == null ? null : BigDecimal.valueOf(sizeMib));
        when(mock.getMemoryDeviceType()).thenReturn(memoryDeviceType);
        when(mock.getSpeedMhz()).thenReturn(achievableSpeedMhz);
        when(mock.getManufacturer()).thenReturn(manufacturer);
        when(mock.getDataWidthBits()).thenReturn(dataWidthBits);
        when(mock.getResourceContext()).thenReturn(resourceContext);
        when(mock.getChassisContext()).thenReturn(chassisContext);

        return mock;
    }

    public static Memory createAvailableMemory(MemoryDeviceType memoryDeviceType, Integer speedMhz, Integer dataWidthBits,
                                                   Integer sizeMib, String manufacturer) {
        return createAvailableMemory(memoryDeviceType, speedMhz, dataWidthBits, sizeMib, manufacturer, null);
    }

    public static Memory createAvailableMemory(MemoryDeviceType memoryDeviceType, Integer speedMhz, Integer dataWidthBits,
                                                   Integer sizeMib, String manufacturer, Id id) {
        Memory mock = mock(Memory.class);
        when(mock.getCapacityMib()).thenReturn(sizeMib);
        when(mock.getMemoryDeviceType()).thenReturn(memoryDeviceType);
        when(mock.getOperatingSpeedMhz()).thenReturn(speedMhz);
        when(mock.getManufacturer()).thenReturn(manufacturer);
        when(mock.getDataWidthBits()).thenReturn(dataWidthBits);
        when(mock.getId()).thenReturn(id);

        return mock;
    }
}
