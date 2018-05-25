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

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.RedfishDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.redfish.OemType;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "redfishVersion", "uuid", "chassis", "systems", "managers", "eventService",
    "fabrics", "storageServices", "telemetryService", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ServiceRootDto extends RedfishDto {
    private final Links links = new Links();
    private final Oem oem = new Oem();
    @JsonIgnore
    private List<UnknownOemDto> unknownOems = new ArrayList<>();
    private String redfishVersion;
    @JsonProperty("UUID")
    private UUID uuid;
    private SingletonContext systems;
    private SingletonContext chassis;
    private SingletonContext managers;
    private SingletonContext fabrics;
    private SingletonContext storageServices;
    private SingletonContext eventService;
    private SingletonContext telemetryService;

    public ServiceRootDto() {
        super("#ServiceRoot.v1_1_1.ServiceRoot");
    }

    public String getRedfishVersion() {
        return redfishVersion;
    }

    public void setRedfishVersion(String redfishVersion) {
        this.redfishVersion = redfishVersion;
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public SingletonContext getSystems() {
        return systems;
    }

    public void setSystems(SingletonContext computerSystems) {
        this.systems = computerSystems;
    }

    public SingletonContext getChassis() {
        return chassis;
    }

    public void setChassis(SingletonContext chassis) {
        this.chassis = chassis;
    }

    public SingletonContext getManagers() {
        return managers;
    }

    public void setManagers(SingletonContext managers) {
        this.managers = managers;
    }

    public SingletonContext getFabrics() {
        return fabrics;
    }

    public void setFabrics(SingletonContext fabrics) {
        this.fabrics = fabrics;
    }

    public SingletonContext getEventService() {
        return eventService;
    }

    public void setEventService(SingletonContext eventService) {
        this.eventService = eventService;
    }

    public SingletonContext getStorageServices() {
        return storageServices;
    }

    public void setStorageServices(SingletonContext storageServices) {
        this.storageServices = storageServices;
    }

    public SingletonContext getTelemetryService() {
        return telemetryService;
    }

    public void setTelemetryService(SingletonContext telemetryService) {
        this.telemetryService = telemetryService;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    @Override
    public void setUnknownOems(List<UnknownOemDto> unknownOems) {
        this.unknownOems = unknownOems;
    }

    public Oem getOem() {
        return oem;
    }

    public Links getLinks() {
        return links;
    }

    public final class Links extends RedfishLinksDto {
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        public void setRackScaleOem(RackScaleOem rackScaleOem) {
            this.rackScaleOem = rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "apiVersion", "composedNodes", "ethernetSwitches"})
        public final class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.ServiceRoot";
            private String apiVersion;
            @JsonProperty("Nodes")
            private SingletonContext composedNodes;
            private SingletonContext ethernetSwitches;

            public String getoDataType() {
                return oDataType;
            }

            public String getApiVersion() {
                return apiVersion;
            }

            public void setApiVersion(String apiVersion) {
                this.apiVersion = apiVersion;
            }

            public SingletonContext getComposedNodes() {
                return composedNodes;
            }

            public void setComposedNodes(SingletonContext composedNodes) {
                this.composedNodes = composedNodes;
            }

            public SingletonContext getEthernetSwitches() {
                return ethernetSwitches;
            }

            public void setEthernetSwitches(SingletonContext ethernetSwitches) {
                this.ethernetSwitches = ethernetSwitches;
            }
        }
    }
}
