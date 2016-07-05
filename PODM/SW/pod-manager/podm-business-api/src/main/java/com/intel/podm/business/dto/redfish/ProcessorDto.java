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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ProcessorIdDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;

import java.util.List;

public final class ProcessorDto {
    private final String name;
    private final String description;
    private final Id id;
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
    private final Context containedBy;
    private final ProcessorIdDto processorId;

    private final Status status;

    private ProcessorDto(Builder builder) {
        name = builder.name;
        description = builder.description;
        id = builder.id;
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
        containedBy = builder.containedBy;
        processorId = builder.processorId;
        status = builder.status;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public Id getId() {
        return id;
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

    public Context getContainedBy() {
        return containedBy;
    }

    public ProcessorIdDto getProcessorId() {
        return processorId;
    }

    public Status getStatus() {
        return status;
    }

    public static final class Builder {
        private String name;
        private String description;
        private Id id;
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
        private Context containedBy;
        private ProcessorIdDto processorId;
        private Status status;

        private Builder() {
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder socket(String val) {
            socket = val;
            return this;
        }

        public Builder processorType(ProcessorType val) {
            processorType = val;
            return this;
        }

        public Builder processorArchitecture(ProcessorArchitecture val) {
            processorArchitecture = val;
            return this;
        }

        public Builder instructionSet(InstructionSet val) {
            instructionSet = val;
            return this;
        }

        public Builder manufacturer(String val) {
            manufacturer = val;
            return this;
        }

        public Builder model(String val) {
            model = val;
            return this;
        }

        public Builder maxSpeedMhz(Integer val) {
            maxSpeedMHz = val;
            return this;
        }

        public Builder totalCores(Integer val) {
            totalCores = val;
            return this;
        }

        public Builder totalThreads(Integer val) {
            totalThreads = val;
            return this;
        }

        public Builder brand(ProcessorBrand val) {
            brand = val;
            return this;
        }

        public Builder capabilities(List<String> val) {
            capabilities = val;
            return this;
        }

        public Builder containedBy(Context val) {
            containedBy = val;
            return this;
        }

        public Builder processorId(ProcessorIdDto val) {
            processorId = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public ProcessorDto build() {
            return new ProcessorDto(this);
        }
    }
}
