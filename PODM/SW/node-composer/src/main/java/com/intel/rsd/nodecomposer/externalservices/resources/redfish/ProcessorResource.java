/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.InstructionSet;
import com.intel.rsd.nodecomposer.types.ProcessorBrand;
import com.intel.rsd.nodecomposer.types.ProcessorType;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;
import lombok.Getter;

import java.util.List;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Processor" + VERSION_PATTERN + "Processor"
})
public class ProcessorResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @Getter
    @JsonSetter(value = "ProcessorType", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<ProcessorType> processorType = unassigned();

    @Getter
    @JsonSetter(value = "InstructionSet", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<InstructionSet> instructionSet = unassigned();

    @Getter
    @JsonSetter(value = "Model", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> model = unassigned();

    @Getter
    @JsonSetter(value = "MaxSpeedMHz", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> maxSpeedMhz = unassigned();

    @Getter
    @JsonSetter(value = "TotalCores", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> totalCores = unassigned();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Ref<ProcessorBrand> getBrand() {
        return oem.rackScaleOem.brand;
    }

    public Ref<Boolean> isErased() {
        return oem.rackScaleOem.fpga.erased;
    }

    public Ref<List<String>> getCapabilities() {
        return oem.rackScaleOem.capabilities;
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("FPGA")
            private Fpga fpga = new Fpga();

            @JsonSetter(value = "Brand", nulls = AS_EMPTY)
            @AsUnassigned(WHEN_NULL)
            private Ref<ProcessorBrand> brand = unassigned();

            @JsonSetter(value = "Capabilities", nulls = AS_EMPTY)
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<String>> capabilities = unassigned();

            public class Fpga {
                @JsonSetter(value = "Erased", nulls = AS_EMPTY)
                @AsUnassigned(WHEN_NULL)
                private Ref<Boolean> erased = unassigned();
            }
        }
    }
}
