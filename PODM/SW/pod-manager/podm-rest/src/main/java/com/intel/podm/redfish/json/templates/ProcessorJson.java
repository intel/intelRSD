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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;
import com.intel.podm.rest.odataid.ODataId;

import java.util.List;

import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "socket", "processorType", "processorArchitecture",
        "instructionSet", "manufacturer", "model", "processorId", "maxSpeedMHz", "totalCores", "totalThreads", "status", "oem"
})
public class ProcessorJson extends BaseJson {
    public String name;
    public String description;
    public Id id;
    public String socket;
    public ProcessorType processorType;
    public ProcessorArchitecture processorArchitecture;
    public InstructionSet instructionSet;
    public String manufacturer;
    public String model;
    public Integer maxSpeedMHz;
    public Integer totalCores;
    public Integer totalThreads;
    public Status status;
    @JsonProperty("ProcessorId")
    public ProcessorIdJson processorId = new ProcessorIdJson();
    public ProcessorOemJson oem = new ProcessorOemJson();

    public ProcessorJson() {
        super("#Processor.1.0.0.Processor");
    }

    @JsonPropertyOrder({
            "vendorId", "identificationRegisters", "effectiveFamily", "effectiveModel", "step", "microcodeInfo"
    })
    public static class ProcessorIdJson {
        @JsonProperty("VendorId")
        public String vendorId;
        public String identificationRegisters;
        public String effectiveFamily;
        public String effectiveModel;
        public String step;
        public String microcodeInfo;
    }

    public static class ProcessorOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"odataType", "brand", "capabilities", "containedBy"})
        public static class RackScaleOem {
            @JsonProperty("@odata.type")
            public String odataType;
            public ProcessorBrand brand;
            public List<String> capabilities = newArrayList();
            public ODataId containedBy;
        }
    }
}
