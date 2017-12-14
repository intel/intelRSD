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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "status", "remoteTargets", "logicalDrives", "drives", "oem", "links"
})
public final class StorageServiceDto extends RedfishDto {
    private Status status;
    private SingletonContext remoteTargets;
    private SingletonContext logicalDrives;
    private SingletonContext drives;
    private Links links = new Links();

    public StorageServiceDto() {
        super("#StorageService.v1_0_0.StorageService");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getRemoteTargets() {
        return remoteTargets;
    }

    public void setRemoteTargets(SingletonContext remoteTargets) {
        this.remoteTargets = remoteTargets;
    }

    public SingletonContext getLogicalDrives() {
        return logicalDrives;
    }

    public void setLogicalDrives(SingletonContext logicalDrives) {
        this.logicalDrives = logicalDrives;
    }

    public SingletonContext getDrives() {
        return drives;
    }

    public void setDrives(SingletonContext drives) {
        this.drives = drives;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    @JsonPropertyOrder({"pcieFunctions", "oem"})
    public class Links extends RedfishLinksDto {
        private Set<Context> managedBy = new HashSet<>();

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }
    }
}
