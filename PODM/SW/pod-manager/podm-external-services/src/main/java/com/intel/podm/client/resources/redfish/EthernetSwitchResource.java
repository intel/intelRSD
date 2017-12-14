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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#EthernetSwitch" + VERSION_PATTERN + "EthernetSwitch"
})
public class EthernetSwitchResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("SwitchId")
    private String switchId;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("ManufacturingDate")
    private String manufacturingDate;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("PartNumber")
    private String partNumber;
    @JsonProperty("FirmwareName")
    private String firmwareName;
    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;
    @JsonProperty("Role")
    private String role;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("ACLs")
    private ODataId acls;
    @JsonProperty("Ports")
    private ODataId ports;
    @JsonProperty("Metrics")
    private ODataId metrics;
    @JsonProperty("Links")
    private Links links = new Links();

    public String getSwitchId() {
        return switchId;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturingDate() {
        return manufacturingDate;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getFirmwareName() {
        return firmwareName;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public String getRole() {
        return role;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("switchAcls")
    public Iterable<ResourceSupplier> getAcls() throws WebClientRequestException {
        return processMembersListResource(acls);
    }

    @LinkName("switchPorts")
    public Iterable<ResourceSupplier> getPorts() throws WebClientRequestException {
        return processMembersListResource(ports);
    }

    @LinkName("switchMetrics")
    public ResourceSupplier getMetrics() throws WebClientRequestException {
        if (metrics == null) {
            return null;
        }
        return toSupplier(metrics);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Chassis")
        private ODataId chassis;
        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;
    }
}
