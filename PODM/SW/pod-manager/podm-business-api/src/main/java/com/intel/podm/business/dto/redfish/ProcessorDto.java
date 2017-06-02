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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ProcessorIdDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class ProcessorDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String socket;
    private final ProcessorType processorType;
    private final ProcessorArchitecture processorArchitecture;
    private final InstructionSet instructionSet;
    private final String manufacturer;
    private final String model;
    private final Integer maxSpeedMhz;
    private final Integer totalCores;
    private final Integer totalThreads;
    private final ProcessorBrand brand;
    private final List<String> capabilities;
    private final ProcessorIdDto processorId;
    private final Status status;

    private ProcessorDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        socket = builder.socket;
        processorType = builder.processorType;
        processorArchitecture = builder.processorArchitecture;
        instructionSet = builder.instructionSet;
        manufacturer = builder.manufacturer;
        model = builder.model;
        maxSpeedMhz = builder.maxSpeedMHz;
        totalCores = builder.totalCores;
        totalThreads = builder.totalThreads;
        brand = builder.brand;
        capabilities = builder.capabilities;
        processorId = builder.processorId;
        status = builder.status;
    }

    public static Builder newBuilder() {
        return new Builder();
    }
    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public String getSocket() {
        return socket;
    }

    public ProcessorType getProcessorType() {
        return processorType;
    }

    public ProcessorArchitecture getProcessorArchitecture() {
        return processorArchitecture;
    }

    public InstructionSet getInstructionSet() {
        return instructionSet;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public Integer getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    public Integer getTotalCores() {
        return totalCores;
    }

    public Integer getTotalThreads() {
        return totalThreads;
    }

    public ProcessorBrand getBrand() {
        return brand;
    }

    public List<String> getCapabilities() {
        return capabilities;
    }

    public ProcessorIdDto getProcessorId() {
        return processorId;
    }

    public Status getStatus() {
        return status;
    }

    @Override
    public Links getLinks() {
        throw new UnsupportedOperationException("Links are not available in this resource");
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String socket;
        private ProcessorType processorType;
        private ProcessorArchitecture processorArchitecture;
        private InstructionSet instructionSet;
        private String manufacturer;
        private String model;
        private Integer maxSpeedMHz;
        private Integer totalCores;
        private Integer totalThreads;
        private ProcessorBrand brand;
        private List<String> capabilities;
        private ProcessorIdDto processorId;
        private Status status;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder socket(String socket) {
            this.socket = socket;
            return this;
        }

        public Builder processorType(ProcessorType processorType) {
            this.processorType = processorType;
            return this;
        }

        public Builder processorArchitecture(ProcessorArchitecture processorArchitecture) {
            this.processorArchitecture = processorArchitecture;
            return this;
        }

        public Builder instructionSet(InstructionSet instructionSet) {
            this.instructionSet = instructionSet;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder maxSpeedMhz(Integer maxSpeedMHz) {
            this.maxSpeedMHz = maxSpeedMHz;
            return this;
        }

        public Builder totalCores(Integer totalCores) {
            this.totalCores = totalCores;
            return this;
        }

        public Builder totalThreads(Integer totalThreads) {
            this.totalThreads = totalThreads;
            return this;
        }

        public Builder brand(ProcessorBrand brand) {
            this.brand = brand;
            return this;
        }

        public Builder capabilities(List<String> capabilities) {
            this.capabilities = capabilities;
            return this;
        }

        public Builder processorId(ProcessorIdDto processorId) {
            this.processorId = processorId;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public ProcessorDto build() {
            return new ProcessorDto(this);
        }
    }
}
