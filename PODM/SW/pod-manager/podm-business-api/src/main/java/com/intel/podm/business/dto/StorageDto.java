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

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Status;

import java.util.Collection;
import java.util.List;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "status", "storageControllers", "drives", "actions"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class StorageDto extends RedfishDto {
    private Status status;
    private List<StorageControllerDto> storageControllers;
    private Set<Context> drives;
    private Links links = new Links();

    public StorageDto() {
        super("#Storage.v1_0_0.Storage");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public List<StorageControllerDto> getStorageControllers() {
        return storageControllers;
    }

    public void setStorageControllers(List<StorageControllerDto> storageControllers) {
        this.storageControllers = storageControllers;
    }

    public Set<Context> getDrives() {
        return drives;
    }

    public void setDrives(Set<Context> drives) {
        this.drives = drives;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    @JsonPropertyOrder({"enclosures"})
    public final class Links extends RedfishLinksDto {
        private Collection<Context> enclosures;

        public Collection<Context> getEnclosures() {
            return enclosures;
        }

        public void setEnclosures(Collection<Context> enclosures) {
            this.enclosures = enclosures;
        }
    }
}
