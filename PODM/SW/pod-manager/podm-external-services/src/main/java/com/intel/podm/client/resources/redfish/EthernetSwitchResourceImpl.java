/*
 * Copyright (c) 2016 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.List;

@OdataTypes({
        "#EthernetSwitch.1.0.0.EthernetSwitch",
        "#EthernetSwitch.v1_0_0.EthernetSwitch"
})
public class EthernetSwitchResourceImpl extends ExternalServiceResourceImpl implements EthernetSwitchResource {
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
    @JsonProperty("Oem")
    private Object oem = new Object();
    @JsonProperty("Ports")
    private ODataId ports;
    @JsonProperty("Links")
    private Links links;

    @Override
    public String getSwitchId() {
        return switchId;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getManufacturingDate() {
        return manufacturingDate;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public String getPartNumber() {
        return partNumber;
    }

    @Override
    public String getFirmwareName() {
        return firmwareName;
    }

    @Override
    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    @Override
    public String getRole() {
        return role;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("switchPorts")
    public Iterable<ResourceSupplier> getPorts() throws ExternalServiceApiReaderException {
        return processMembersListResource(ports);
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Links {
        @JsonProperty("Chassis")
        private ODataId chassis;
        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;
        @JsonProperty("Oem")
        private Object oem = new Object();
    }
}
