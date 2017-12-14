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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Status;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "links", "oem"})
public final class ZoneDto extends RedfishDto {
    private Status status;
    private Links links = new Links();

    public ZoneDto() {
        super("#Zone.v1_0_0.Zone");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    @JsonPropertyOrder({"endpoints", "involvedSwitches"})
    public final class Links extends RedfishLinksDto {
        private Set<Context> endpoints = new HashSet<>();
        private Set<Context> involvedSwitches = new HashSet<>();

        public Set<Context> getEndpoints() {
            return endpoints;
        }

        public void setEndpoints(Set<Context> endpoints) {
            this.endpoints = endpoints;
        }

        public Set<Context> getInvolvedSwitches() {
            return involvedSwitches;
        }

        public void setInvolvedSwitches(Set<Context> involvedSwitches) {
            this.involvedSwitches = involvedSwitches;
        }
    }
}
