/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#Processor" + VERSION_PATTERN + "Processor"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ProcessorResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Socket")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> socket = unassigned();
    @JsonProperty("ProcessorType")
    @AsUnassigned(WHEN_NULL)
    private Ref<ProcessorType> processorType = unassigned();
    @JsonProperty("ProcessorArchitecture")
    @AsUnassigned(WHEN_NULL)
    private Ref<ProcessorArchitecture> processorArchitecture = unassigned();
    @JsonProperty("InstructionSet")
    @AsUnassigned(WHEN_NULL)
    private Ref<InstructionSet> instructionSet = unassigned();
    @JsonProperty("Manufacturer")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> manufacturer = unassigned();
    @JsonProperty("Model")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> model = unassigned();
    @JsonProperty("MaxSpeedMHz")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> maxSpeedMhz = unassigned();
    @JsonProperty("TotalCores")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> totalCores = unassigned();
    @JsonProperty("TotalThreads")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> totalThreads = unassigned();
    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();
    @JsonProperty("ProcessorId")
    private ProcessorIdObject processorId;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Ref<String> getSocket() {
        return socket;
    }

    public Ref<ProcessorType> getProcessorType() {
        return processorType;
    }

    public Ref<ProcessorArchitecture> getProcessorArchitecture() {
        return processorArchitecture;
    }

    public Ref<InstructionSet> getInstructionSet() {
        return instructionSet;
    }

    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    public Ref<String> getModel() {
        return model;
    }

    public Ref<Integer> getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    public Ref<Integer> getTotalCores() {
        return totalCores;
    }

    public Ref<Integer> getTotalThreads() {
        return totalThreads;
    }

    public Ref<ProcessorBrand> getBrand() {
        return oem.rackScaleOem.brand;
    }

    public Ref<List<String>> getCapabilities() {
        return oem.rackScaleOem.capabilities;
    }

    public Ref<Set<EndpointResource>> getEndpoints() {
        return oem.rackScaleOem.endpoints;
    }

    public Ref<List<ProcessorMemoryObject>> getProcessorMemory() {
        return oem.rackScaleOem.onPackageMemory;
    }

    public Ref<BigDecimal> getThermalDesignPowerWatt() {
        return oem.rackScaleOem.thermalDesignPowerWatt;
    }

    public Ref<FpgaObject> getFpgaObject() {
        return oem.rackScaleOem.fpga;
    }

    public ProcessorIdObject getProcessorId() {
        return processorId;
    }

    public Ref<Status> getStatus() {
        return status;
    }

    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @LinkName("processorMetrics")
    public ResourceSupplier getProcessorMetrics() {
        if (oem.rackScaleOem.processorMetrics == null) {
            return null;
        }
        return toSupplier(oem.rackScaleOem.processorMetrics);
    }

    public Ref<Map<String, String>> getExtendedIdentificationRegisters() {
        return oem.rackScaleOem.extendedIdentificationRegisters;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Capabilities")
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<String>> capabilities = unassigned();

            @JsonProperty("Brand")
            @AsUnassigned(WHEN_NULL)
            private Ref<ProcessorBrand> brand = unassigned();

            @JsonProperty("Endpoints")
            @AsUnassigned(WHEN_NULL)
            private Ref<Set<EndpointResource>> endpoints = unassigned();

            @JsonProperty("OnPackageMemory")
            @AsUnassigned(WHEN_NULL)
            private Ref<List<ProcessorMemoryObject>> onPackageMemory = unassigned();

            @JsonProperty("ThermalDesignPowerWatt")
            @AsUnassigned(WHEN_NULL)
            private Ref<BigDecimal> thermalDesignPowerWatt = unassigned();

            @JsonProperty("FPGA")
            @AsUnassigned(WHEN_NULL)
            private Ref<FpgaObject> fpga = unassigned();

            @JsonProperty("Metrics")
            private ODataId processorMetrics;

            @JsonProperty("ExtendedIdentificationRegisters")
            @AsUnassigned(WHEN_NULL)
            private Ref<Map<String, String>> extendedIdentificationRegisters = unassigned();
        }
    }
}
