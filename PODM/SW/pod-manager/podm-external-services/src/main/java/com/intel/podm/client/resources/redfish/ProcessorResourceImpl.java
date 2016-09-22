/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.LuiProcessorResource;
import com.intel.podm.client.api.resources.redfish.ProcessorIdObject;
import com.intel.podm.client.api.resources.redfish.ProcessorResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;

import java.util.List;

@OdataTypes({
        "#Processor.1.0.0.Processor",
        "#Processor.v1_0_0.Processor"
})
public class ProcessorResourceImpl extends ExternalServiceResourceImpl implements ProcessorResource, LuiProcessorResource {
    @JsonProperty("Socket")
    private String socket;
    @JsonProperty("ProcessorType")
    private ProcessorType processorType;
    @JsonProperty("ProcessorArchitecture")
    private ProcessorArchitecture processorArchitecture;
    @JsonProperty("InstructionSet")
    private InstructionSet instructionSet;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("MaxSpeedMHz")
    private Integer maxSpeedMhz;
    @JsonProperty("TotalCores")
    private Integer totalCores;
    @JsonProperty("TotalThreads")
    private Integer totalThreads;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("ProcessorId")
    private ProcessorIdObjectImpl processorId;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public String getSocket() {
        return socket;
    }

    @Override
    public ProcessorType getProcessorType() {
        return processorType;
    }

    @Override
    public ProcessorArchitecture getProcessorArchitecture() {
        return processorArchitecture;
    }

    @Override
    public InstructionSet getInstructionSet() {
        return instructionSet;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public Integer getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    @Override
    public Integer getTotalCores() {
        return totalCores;
    }

    @Override
    public Integer getTotalThreads() {
        return totalThreads;
    }

    @Override
    public ProcessorBrand getBrand() {
        return oem.rackScaleOem.brand;
    }

    @Override
    public List<String> getCapabilities() {
        return oem.rackScaleOem.capabilities;
    }

    @Override
    public ProcessorIdObject getProcessorId() {
        return processorId;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        return toSupplier(oem.rackScaleOem.containedBy);
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonIgnoreProperties(ignoreUnknown = true)
        private static final class RackScaleOem {
            @JsonProperty("Capabilities")
            private List<String> capabilities;
            @JsonProperty("Brand")
            private ProcessorBrand brand;
            @JsonProperty("ContainedBy")
            private ODataId containedBy;
        }
    }
}
