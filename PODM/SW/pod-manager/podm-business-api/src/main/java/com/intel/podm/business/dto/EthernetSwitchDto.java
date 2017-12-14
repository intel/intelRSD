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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;

import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@SuppressWarnings({"checkstyle:MethodCount"})
@JsonPropertyOrder({"@odata.type", "id", "switchId", "name", "description", "manufacturer", "model",
    "manufacturingDate", "serialNumber", "partNumber", "firmwareName", "firmwareVersion", "role", "status", "acls", "oem",
    "ports", "links"
})
public final class EthernetSwitchDto extends RedfishDto {
    private String switchId;
    private String manufacturer;
    private String model;
    private String manufacturingDate;
    private String serialNumber;
    private String partNumber;
    private String firmwareName;
    private String firmwareVersion;
    private String role;
    private Status status;
    @JsonProperty("ACLs")
    private SingletonContext acls;
    private SingletonContext ports;
    @JsonInclude(NON_NULL)
    private SingletonContext metrics;
    private Links links;

    public EthernetSwitchDto() {
        super("#EthernetSwitch.v1_1_0.EthernetSwitch");
    }

    public String getSwitchId() {
        return switchId;
    }

    public void setSwitchId(String switchId) {
        this.switchId = switchId;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getManufacturingDate() {
        return manufacturingDate;
    }

    public void setManufacturingDate(String manufacturingDate) {
        this.manufacturingDate = manufacturingDate;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getFirmwareName() {
        return firmwareName;
    }

    public void setFirmwareName(String firmwareName) {
        this.firmwareName = firmwareName;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public String getRole() {
        return role;
    }

    public void setRole(String role) {
        this.role = role;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getAcls() {
        return acls;
    }

    public void setAcls(SingletonContext acls) {
        this.acls = acls;
    }

    public SingletonContext getPorts() {
        return ports;
    }

    public void setPorts(SingletonContext ports) {
        this.ports = ports;
    }

    public SingletonContext getMetrics() {
        return metrics;
    }

    public void setMetrics(SingletonContext metrics) {
        this.metrics = metrics;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    public Links createLinks(Context chassis, Set<Context> managedBy) {
        return new Links(chassis, managedBy);
    }

    private final class Links extends RedfishLinksDto {
        private final Context chassis;
        private final Set<Context> managedBy;

        Links(Context chassis, Set<Context> managedBy) {
            this.chassis = chassis;
            this.managedBy = managedBy;
        }

        public Context getChassis() {
            return chassis;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }
    }
}
