/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
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

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Switch" + VERSION_PATTERN + "Switch"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class SwitchResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
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
    private List<RedundancyItem> redundancy = new ArrayList<>();
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

    public Set<ResetType> getAllowableResetTypes() {
        return actions.reset.allowableValues;
    }

    public Protocol getSwitchType() {
        return switchType;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getSku() {
        return sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public Integer getDomainId() {
        return domainId;
    }

    public Boolean getIsManaged() {
        return isManaged;
    }

    public Integer getTotalSwitchWidth() {
        return totalSwitchWidth;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("portsInFabricSwitch")
    public Iterable<ResourceSupplier> getPorts() throws WebClientRequestException {
        return processMembersListResource(ports);
    }

    @LinkName("chassisInSwitch")
    public Iterable<ResourceSupplier> getChassis() throws WebClientRequestException {
        return toSuppliers(links.chassis);
    }

    @LinkName("managedByInSwitch")
    public Iterable<ResourceSupplier> getManagedBy() throws WebClientRequestException {
        return toSuppliers(links.managedBy);
    }

    public List<RedundancyItem> getRedundancy() {
        return redundancy;
    }

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
