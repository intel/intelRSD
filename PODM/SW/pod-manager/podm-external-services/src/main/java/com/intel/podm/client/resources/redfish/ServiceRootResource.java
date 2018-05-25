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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.redfish.OemType;

import java.util.UUID;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static java.util.Optional.ofNullable;

@OdataTypes({"#ServiceRoot" + VERSION_PATTERN + "ServiceRoot"})
public class ServiceRootResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("UUID")
    private UUID uuid;

    @JsonProperty("Systems")
    private ODataId computerSystems;

    @JsonProperty("Chassis")
    private ODataId chassis;

    @JsonProperty("Managers")
    private ODataId managers;

    /**
     * @deprecated Left for backwards compatibility only, removed in RackScale API version 2.2.0
     */
    @Deprecated
    @JsonProperty("EthernetSwitches")
    private ODataId ethernetSwitches;

    /**
     * @deprecated Left for backwards compatibility only, removed in RackScale API version 2.2.0
     */
    @Deprecated
    @JsonProperty("Services")
    private ODataId services;

    @JsonProperty("Fabrics")
    private ODataId fabrics;

    @JsonProperty("StorageServices")
    private ODataId storageServices;

    @JsonProperty("TelemetryService")
    private ODataId telemetryService;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public UUID getUuid() {
        return uuid;
    }

    public String getApiVersion() {
        return oem.rackScaleOem.apiVersion;
    }

    @LinkName("computerSystems")
    public Iterable<ResourceSupplier> getComputerSystems() throws WebClientRequestException {
        return processMembersListResource(computerSystems);
    }

    @LinkName("chassis")
    public Iterable<ResourceSupplier> getChassis() throws WebClientRequestException {
        return processMembersListResource(chassis);
    }

    @LinkName("managers")
    public Iterable<ResourceSupplier> getManagers() throws WebClientRequestException {
        return processMembersListResource(managers);
    }

    @LinkName("ethernetSwitches")
    public Iterable<ResourceSupplier> getEthernetSwitches() throws WebClientRequestException {
        // Backwards compatibility fallback starting from RackScale API version 2.2.0
        return processMembersListResource(ofNullable(oem.rackScaleOem.ethernetSwitches).orElse(ethernetSwitches));
    }

    @LinkName("services")
    public Iterable<ResourceSupplier> getServices() throws WebClientRequestException {
        // Backwards compatibility fallback starting from RackScale API version 2.2.0
        return processMembersListResource(ofNullable(oem.rackScaleOem.services).orElse(services));
    }

    @LinkName("fabrics")
    public Iterable<ResourceSupplier> getFabrics() throws WebClientRequestException {
        return processMembersListResource(fabrics);
    }

    @LinkName("storageServices")
    public Iterable<ResourceSupplier> getStorageServices() throws WebClientRequestException {
        return processMembersListResource(storageServices);
    }

    @LinkName("telemetryService")
    public ResourceSupplier getTelemetryService() throws WebClientRequestException {
        return telemetryService != null
            ? toSupplier(telemetryService)
            : null;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("ApiVersion")
            private String apiVersion;

            @JsonProperty("EthernetSwitches")
            private ODataId ethernetSwitches;

            @JsonProperty("Services")
            private ODataId services;
        }
    }
}
