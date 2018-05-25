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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ProtocolType;

import java.util.ArrayList;
import java.util.List;

public class DcbxConfigObject {
    @JsonProperty("ApplicationProtocol")
    private List<ApplicationProtocolTypeObject> applicationProtocolType = new ArrayList<>();
    @JsonProperty("PriorityToPriorityGroupMapping")
    private List<PriorityClassMappingObject> priorityToPriorityGroupMapping = new ArrayList<>();
    @JsonProperty("BandwidthAllocation")
    private List<BandwidthMappingObject> bandwidthAllocation = new ArrayList<>();

    public List<ApplicationProtocolTypeObject> getApplicationProtocolType() {
        return applicationProtocolType;
    }

    public List<PriorityClassMappingObject> getPriorityToPriorityGroupMapping() {
        return priorityToPriorityGroupMapping;
    }

    public List<BandwidthMappingObject> getBandwidthAllocation() {
        return bandwidthAllocation;
    }

    public static class ApplicationProtocolTypeObject {
        @JsonProperty("Priority")
        private Integer priority;

        @JsonProperty("Protocol")
        private ProtocolType protocol;

        @JsonProperty("Port")
        private Integer port;

        public Integer getPriority() {
            return priority;
        }

        public ProtocolType getProtocol() {
            return protocol;
        }

        public Integer getPort() {
            return port;
        }
    }

    public static class PriorityClassMappingObject {
        @JsonProperty("PriorityGroup")
        private Integer priorityGroup;
        @JsonProperty("Priority")
        private Integer priority;

        public Integer getPriorityGroup() {
            return priorityGroup;
        }

        public Integer getPriority() {
            return priority;
        }
    }

    public static class BandwidthMappingObject {
        @JsonProperty("PriorityGroup")
        private Integer priorityGroup;

        @JsonProperty("BandwidthPercent")
        private Integer bandwidthPrecent;

        public Integer getPriorityGroup() {
            return priorityGroup;
        }

        public Integer getBandwidthPrecent() {
            return bandwidthPrecent;
        }
    }
}

