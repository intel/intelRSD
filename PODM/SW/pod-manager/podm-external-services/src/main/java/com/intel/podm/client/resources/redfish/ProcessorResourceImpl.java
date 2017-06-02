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
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ProcessorIdObject;
import com.intel.podm.client.api.resources.redfish.ProcessorResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OemType;

import java.util.List;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#Processor" + OdataTypes.VERSION_PATTERN + "Processor"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ProcessorResourceImpl extends ExternalServiceResourceImpl implements ProcessorResource {
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
    private ProcessorIdObjectImpl processorId;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public Ref<String> getSocket() {
        return socket;
    }

    @Override
    public Ref<ProcessorType> getProcessorType() {
        return processorType;
    }

    @Override
    public Ref<ProcessorArchitecture> getProcessorArchitecture() {
        return processorArchitecture;
    }

    @Override
    public Ref<InstructionSet> getInstructionSet() {
        return instructionSet;
    }

    @Override
    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    @Override
    public Ref<String> getModel() {
        return model;
    }

    @Override
    public Ref<Integer> getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    @Override
    public Ref<Integer> getTotalCores() {
        return totalCores;
    }

    @Override
    public Ref<Integer> getTotalThreads() {
        return totalThreads;
    }

    @Override
    public Ref<ProcessorBrand> getBrand() {
        return oem.rackScaleOem.brand;
    }

    @Override
    public Ref<List<String>> getCapabilities() {
        return oem.rackScaleOem.capabilities;
    }

    @Override
    public ProcessorIdObject getProcessorId() {
        return processorId;
    }

    @Override
    public Ref<Status> getStatus() {
        return status;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
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
        }
    }
}
