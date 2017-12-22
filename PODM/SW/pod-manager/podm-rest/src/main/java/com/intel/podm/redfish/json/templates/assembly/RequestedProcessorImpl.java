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

package com.intel.podm.redfish.json.templates.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.deserialization.PositiveIntegerDeserializer;

import java.util.List;

import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;

public final class RequestedProcessorImpl implements RequestedNode.Processor {
    @JsonProperty
    private String model;

    @JsonProperty
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer totalCores;

    @JsonProperty("AchievableSpeedMHz")
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer achievableSpeedMhz;

    @JsonProperty
    private InstructionSet instructionSet;

    @JsonProperty
    private ProcessorOem oem = new ProcessorOem();

    @JsonProperty
    private ProcessorType processorType;

    private Context resourceContext;

    private Context chassisContext;

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public ProcessorBrand getBrand() {
        return oem.brand;
    }

    @Override
    public Integer getTotalCores() {
        return totalCores;
    }

    @Override
    public Integer getAchievableSpeedMhz() {
        return achievableSpeedMhz;
    }

    @Override
    public InstructionSet getInstructionSet() {
        return instructionSet;
    }

    @Override
    public ProcessorType getProcessorType() {
        return processorType;
    }

    @JsonProperty("Resource")
    public void setResourceContext(ODataId resource) {
        if (resource == null) {
            return;
        }

        resourceContext = getContextFromUri(resource.toUri(), PROCESSOR);
    }

    @Override
    public Context getResourceContext() {
        return resourceContext;
    }

    @JsonProperty("Chassis")
    public void setChassisContext(ODataId chassis) {
        if (chassis == null) {
            return;
        }

        chassisContext = getContextFromUri(chassis.toUri(), CHASSIS);
    }

    @Override
    public Context getChassisContext() {
        return chassisContext;
    }

    @Override
    public List<String> getCapabilities() {
        return oem.capabilities;
    }

    private static final class ProcessorOem {
        @JsonProperty
        private ProcessorBrand brand;

        @JsonProperty
        private List<String> capabilities;
    }
}
