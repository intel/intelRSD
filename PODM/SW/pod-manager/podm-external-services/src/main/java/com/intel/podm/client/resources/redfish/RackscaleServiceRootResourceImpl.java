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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.RackscaleServiceRootResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.redfish.OemType;

import java.util.UUID;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#ServiceRoot" + OdataTypes.VERSION_PATTERN + "ServiceRoot"
})
public class RackscaleServiceRootResourceImpl extends ExternalServiceResourceImpl implements RackscaleServiceRootResource {
    @JsonProperty("UUID")
    private UUID uuid;

    @JsonProperty("Systems")
    private ODataId computerSystems;

    @JsonProperty("Chassis")
    private ODataId chassis;

    @JsonProperty("Managers")
    private ODataId managers;

    @JsonProperty("EthernetSwitches")
    private ODataId ethernetSwitches;

    @JsonProperty("Services")
    private ODataId services;

    @JsonProperty("Fabrics")
    private ODataId fabrics;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public UUID getUuid() {
        return uuid;
    }

    @Override
    public String getApiVersion() {
        return oem.rackScaleOem.apiVersion;
    }

    @Override
    @LinkName("computerSystems")
    public Iterable<ResourceSupplier> getComputerSystems() throws ExternalServiceApiReaderException {
        return processMembersListResource(computerSystems);
    }

    @Override
    @LinkName("chassis")
    public Iterable<ResourceSupplier> getChassis() throws ExternalServiceApiReaderException {
        return processMembersListResource(chassis);
    }

    @Override
    @LinkName("managers")
    public Iterable<ResourceSupplier> getManagers() throws ExternalServiceApiReaderException {
        return processMembersListResource(managers);
    }

    @Override
    @LinkName("ethernetSwitches")
    public Iterable<ResourceSupplier> getEthernetSwitches() throws ExternalServiceApiReaderException {
        return processMembersListResource(ethernetSwitches);
    }

    @Override
    @LinkName("services")
    public Iterable<ResourceSupplier> getServices() throws ExternalServiceApiReaderException {
        return processMembersListResource(services);
    }

    @Override
    @LinkName("fabrics")
    public Iterable<ResourceSupplier> getFabrics() throws ExternalServiceApiReaderException {
        return processMembersListResource(fabrics);
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("ApiVersion")
            private String apiVersion;
        }
    }
}
