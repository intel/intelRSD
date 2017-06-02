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
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.SwitchResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

@OdataTypes({
    "#Switch" + OdataTypes.VERSION_PATTERN + "Switch"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class SwitchResourceImpl extends ExternalServiceResourceImpl implements SwitchResource {
    @JsonProperty("Id")
    private String id;
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("SwitchType")
    private Protocol switchType;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("SKU")
    private String sku;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("PartNumber")
    private String partNumber;
    @JsonProperty("AssetTag")
    private String assetTag;
    @JsonProperty("DomainID")
    private Integer domainId;
    @JsonProperty("IsManaged")
    private Boolean isManaged;
    @JsonProperty("TotalSwitchWidth")
    private Integer totalSwitchWidth;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    @JsonProperty("PowerState")
    private PowerState powerState;
    @JsonProperty("Ports")
    private ODataId ports;
    @JsonProperty("LogServices")
    private Object logServices = new Object();
    @JsonProperty("Redundancy")
    private List<RedundancyObjectImpl> redundancy = new ArrayList<>();
    @JsonProperty("Links")
    private Links links = new Links();
    @JsonProperty("Actions")
    private Actions actions = new Actions();

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public Set<ResetType> getAllowableResetTypes() {
        return actions.reset.allowableValues;
    }

    @Override
    public Protocol getSwitchType() {
        return switchType;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getSku() {
        return sku;
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
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public Integer getDomainId() {
        return domainId;
    }

    @Override
    public Boolean getManaged() {
        return isManaged;
    }

    @Override
    public Integer getTotalSwitchWidth() {
        return totalSwitchWidth;
    }

    @Override
    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public PowerState getPowerState() {
        return powerState;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("portsInFabricSwitch")
    public Iterable<ResourceSupplier> getPorts() throws ExternalServiceApiReaderException {
        return processMembersListResource(ports);
    }

    @Override
    @LinkName("chassisInSwitch")
    public Iterable<ResourceSupplier> getChassis() throws ExternalServiceApiReaderException {
        return toSuppliers(links.chassis);
    }

    @Override
    @LinkName("managedByInSwitch")
    public Iterable<ResourceSupplier> getManagedBy() throws ExternalServiceApiReaderException {
        return toSuppliers(links.managedBy);
    }

    public List<RedundancyObjectImpl> getRedundancy() {
        return redundancy;
    }

    @JsonPropertyOrder({"chassis", "managedBy", "oem"})
    public class Links extends RedfishLinks {
        @JsonProperty("Chassis")
        private Set<ODataId> chassis;
        @JsonProperty("ManagedBy")
        private Set<ODataId> managedBy;
    }

    public class Actions extends RedfishActions {
        @JsonProperty("#Switch.Reset")
        private Reset reset = new Reset();

        @JsonPropertyOrder({"target", "allowableValues"})
        public class Reset {
            @JsonProperty("target")
            private String target;
            @JsonProperty("ResetType@Redfish.AllowableValues")
            private Set<ResetType> allowableValues;
        }
    }
}
