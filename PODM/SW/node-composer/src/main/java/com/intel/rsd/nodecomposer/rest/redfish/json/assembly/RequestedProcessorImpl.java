/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.json.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.types.Connectivity;
import com.intel.rsd.nodecomposer.types.InstructionSet;
import com.intel.rsd.nodecomposer.types.ProcessorBrand;
import com.intel.rsd.nodecomposer.types.ProcessorType;
import com.intel.rsd.nodecomposer.types.deserialization.PositiveIntegerDeserializer;

import java.io.Serializable;
import java.util.List;

public final class RequestedProcessorImpl implements RequestedNode.Processor, Serializable {
    private static final long serialVersionUID = 5415426715816252284L;

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

    @JsonProperty("Resource")
    private ODataId resourceODataId;

    @JsonProperty("Chassis")
    private ODataId chassisODataId;

    @JsonProperty("Connectivity")
    private List<Connectivity> connectivity;

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public ProcessorBrand getBrand() {
        return oem == null ? null : oem.intelRackScale.brand;
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

    @Override
    public ODataId getResourceODataId() {
        return resourceODataId;
    }

    @Override
    public ODataId getChassisODataId() {
        return chassisODataId;
    }

    @Override
    public List<String> getCapabilities() {
        return oem == null ? null : oem.intelRackScale.capabilities;
    }

    @Override
    public List<Connectivity> getConnectivity() {
        return connectivity;
    }

    private static final class ProcessorOem implements Serializable {
        private static final long serialVersionUID = -7314574549240405099L;

        @JsonProperty("Intel_RackScale")
        private IntelRackScale intelRackScale = new IntelRackScale();

        private static final class IntelRackScale implements Serializable {
            @JsonProperty
            private ProcessorBrand brand;

            @JsonProperty
            private List<String> capabilities;
        }
    }
}
