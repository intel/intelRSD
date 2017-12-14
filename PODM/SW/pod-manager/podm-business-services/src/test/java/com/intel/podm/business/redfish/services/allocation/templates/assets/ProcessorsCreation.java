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

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorBrand;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@SuppressWarnings({"checkstyle:ParameterNumber"})
public final class ProcessorsCreation {
    private ProcessorsCreation() {
    }

    public static RequestedNode.Processor createRequestedProcessor(String model, ProcessorBrand brand, Integer achievableSpeedMhz,
                                                                   InstructionSet instructionSet, Integer totalCores) {
        return createRequestedProcessor(model, brand, achievableSpeedMhz, instructionSet, totalCores, null, null);
    }

    public static RequestedNode.Processor createRequestedProcessor(String model, ProcessorBrand brand, Integer achievableSpeedMhz,
                                                                   InstructionSet instructionSet, Integer totalCores, Context resourceContext) {
        return createRequestedProcessor(model, brand, achievableSpeedMhz, instructionSet, totalCores, resourceContext, null);
    }

    public static RequestedNode.Processor createRequestedProcessor(String model, ProcessorBrand brand, Integer achievableSpeedMhz,
                                                                   InstructionSet instructionSet, Integer totalCores,
                                                                   Context resourceContext, Context chassisContext) {
        RequestedNode.Processor mock = mock(RequestedNode.Processor.class);
        when(mock.getModel()).thenReturn(model);
        when(mock.getBrand()).thenReturn(brand);
        when(mock.getAchievableSpeedMhz()).thenReturn(achievableSpeedMhz);
        when(mock.getInstructionSet()).thenReturn(instructionSet);
        when(mock.getTotalCores()).thenReturn(totalCores);
        when(mock.getResourceContext()).thenReturn(resourceContext);
        when(mock.getChassisContext()).thenReturn(chassisContext);

        return mock;
    }

    public static Processor createAvailableProcessor(String model, ProcessorBrand brand, Integer maxSpeedMhz, InstructionSet instructionSet,
                                                     Integer totalCores) {
        return createAvailableProcessor(model, brand, maxSpeedMhz, instructionSet, totalCores, null);
    }

    public static Processor createAvailableProcessor(String model, ProcessorBrand brand, Integer maxSpeedMhz, InstructionSet instructionSet,
                                                     Integer totalCores, Id id) {
        Processor mock = mock(Processor.class);
        when(mock.getModel()).thenReturn(model);
        when(mock.getBrand()).thenReturn(brand);
        when(mock.getMaxSpeedMhz()).thenReturn(maxSpeedMhz);
        when(mock.getInstructionSet()).thenReturn(instructionSet);
        when(mock.getTotalCores()).thenReturn(totalCores);
        when(mock.getId()).thenReturn(id);

        return mock;
    }
}
