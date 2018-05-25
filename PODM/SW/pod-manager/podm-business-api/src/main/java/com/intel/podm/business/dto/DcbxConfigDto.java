/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.ProtocolType;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({"applicationProtocol", "priorityToPriorityGroupMapping", "bandwidthAllocation"})
public final class DcbxConfigDto {

    private List<ApplicationProtocolDto> applicationProtocol = new ArrayList<>();
    private List<PriorityClassMappingDto> priorityToPriorityGroupMapping = new ArrayList<>();
    private List<BandwidthMappingDto> bandwidthAllocation = new ArrayList<>();

    public List<ApplicationProtocolDto> getApplicationProtocol() {
        return applicationProtocol;
    }

    public void setApplicationProtocol(List<ApplicationProtocolDto> applicationProtocol) {
        this.applicationProtocol = applicationProtocol;
    }

    public List<PriorityClassMappingDto> getPriorityToPriorityGroupMapping() {
        return priorityToPriorityGroupMapping;
    }

    public void setPriorityToPriorityGroupMapping(List<PriorityClassMappingDto> priorityToPriorityGroupMapping) {
        this.priorityToPriorityGroupMapping = priorityToPriorityGroupMapping;
    }

    public List<BandwidthMappingDto> getBandwidthAllocation() {
        return bandwidthAllocation;
    }

    public void setBandwidthAllocation(List<BandwidthMappingDto> bandwidthMapping) {
        this.bandwidthAllocation = bandwidthMapping;
    }

    @JsonPropertyOrder({"priority", "protocol", "port"})
    public static final class ApplicationProtocolDto {
            private Integer priority;
            private ProtocolType protocol;
            private Integer port;

        public Integer getPriority() {
            return priority;
        }

        public void setPriority(Integer priority) {
            this.priority = priority;
        }

        public ProtocolType getProtocol() {
            return protocol;
        }

        public void setProtocol(ProtocolType protocol) {
            this.protocol = protocol;
        }

        public Integer getPort() {
            return port;
        }

        public void setPort(Integer port) {
            this.port = port;
        }
    }

    @JsonPropertyOrder({"priorityGroup", "priority"})
    public static final class PriorityClassMappingDto {
        private Integer priorityGroup;
        private Integer priority;

        public Integer getPriorityGroup() {
            return priorityGroup;
        }

        public void setPriorityGroup(Integer priorityGroup) {
            this.priorityGroup = priorityGroup;
        }

        public Integer getPriority() {
            return priority;
        }

        public void setPriority(Integer priority) {
            this.priority = priority;
        }
    }

    @JsonPropertyOrder({"priorityGroup", "bandwidthPercent"})
    public static final class BandwidthMappingDto {
        private Integer priorityGroup;
        private Integer bandwidthPercent;

        public Integer getPriorityGroup() {
            return priorityGroup;
        }

        public void setPriorityGroup(Integer priorityGroup) {
            this.priorityGroup = priorityGroup;
        }

        public Integer getBandwidthPercent() {
            return bandwidthPercent;
        }

        public void setBandwidthPercent(Integer bandwidthPercent) {
            this.bandwidthPercent = bandwidthPercent;
        }
    }
}
