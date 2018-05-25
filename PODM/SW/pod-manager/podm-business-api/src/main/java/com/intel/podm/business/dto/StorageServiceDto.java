/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "drives", "storagePools",
    "volumes", "endpoints", "links", "oem"
})
@JsonInclude(NON_NULL)
@SuppressWarnings({"checkstyle:MethodCount"})
public final class StorageServiceDto extends RedfishDto {
    private final Links links = new Links();
    private Status status;
    private SingletonContext volumes;
    private SingletonContext storagePools;
    private SingletonContext drives;
    private SingletonContext endpoints;

    public StorageServiceDto() {
        super("#StorageService.v1_0_0.StorageService");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getVolumes() {
        return volumes;
    }

    public void setVolumes(SingletonContext volumes) {
        this.volumes = volumes;
    }

    public SingletonContext getDrives() {
        return drives;
    }

    public void setDrives(SingletonContext drives) {
        this.drives = drives;
    }

    public SingletonContext getEndpoints() {
        return endpoints;
    }

    public void setEndpoints(SingletonContext endpoints) {
        this.endpoints = endpoints;
    }

    public SingletonContext getStoragePools() {
        return storagePools;
    }

    public void setStoragePools(SingletonContext storagePools) {
        this.storagePools = storagePools;
    }

    public Links getLinks() {
        return links;
    }

    @JsonPropertyOrder({"hostingSystem", "oem"})
    public class Links extends RedfishLinksDto {
        private Context hostingSystem;
        private Oem oem = new Oem();

        public Context getHostingSystem() {
            return hostingSystem;
        }

        public void setHostingSystem(Context hostingSystem) {
            this.hostingSystem = hostingSystem;
        }

        public Oem getOem() {
            return oem;
        }

        public void setOem(Oem oem) {
            this.oem = oem;
        }

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemDto {
            @JsonProperty("Intel_RackScale")
            private final RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonPropertyOrder({"managedBy"})
            public class RackScaleOem {
                private Set<Context> managedBy = new HashSet<>();

                public Set<Context> getManagedBy() {
                    return managedBy;
                }

                public void setManagedBy(Set<Context> managedBy) {
                    this.managedBy = managedBy;
                }
            }
        }
    }
}
