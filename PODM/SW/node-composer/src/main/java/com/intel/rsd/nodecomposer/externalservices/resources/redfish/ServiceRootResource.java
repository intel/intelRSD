/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;

import java.util.UUID;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({"#ServiceRoot" + VERSION_PATTERN + "ServiceRoot"})
public class ServiceRootResource extends ExternalServiceResourceImpl {
    @JsonProperty("UUID")
    private UUID uuid;

    @JsonProperty("Systems")
    private ODataId computerSystems;

    @JsonProperty("Chassis")
    private ODataId chassis;

    @JsonProperty("Managers")
    private ODataId managers;

    @JsonProperty("Fabrics")
    private ODataId fabrics;

    @JsonProperty("StorageServices")
    private ODataId storageServices;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public UUID getUuid() {
        return uuid;
    }

    @LinkName("computerSystems")
    public Iterable<ResourceSupplier> getComputerSystems() throws WebClientRequestException {
        return processMembersListResource(computerSystems);
    }

    @LinkName("chassis")
    public Iterable<ResourceSupplier> getChassis() throws WebClientRequestException {
        return processMembersListResource(chassis);
    }

    @LinkName("ethernetSwitches")
    public Iterable<ResourceSupplier> getEthernetSwitches() throws WebClientRequestException {
        return processMembersListResource(oem.rackScaleOem.ethernetSwitches);
    }

    @LinkName("managers")
    public Iterable<ResourceSupplier> getManagers() throws WebClientRequestException {
        return processMembersListResource(managers);
    }

    @LinkName("fabrics")
    public Iterable<ResourceSupplier> getFabrics() throws WebClientRequestException {
        return processMembersListResource(fabrics);
    }

    @LinkName("storageServices")
    public Iterable<ResourceSupplier> getStorageServices() throws WebClientRequestException {
        return processMembersListResource(storageServices);
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("EthernetSwitches")
            private ODataId ethernetSwitches;
        }
    }
}
