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
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.client.resources.redfish.properties.OemResource;
import com.intel.podm.client.resources.redfish.properties.OemVendorImpl;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static java.util.Objects.isNull;

@OdataTypes({
        "#Chassis.1.0.0.Chassis",
        "#Chassis.1.0.1.Chassis",
        "#Chassis.1.1.0.Chassis",
        "#Chassis.1.1.2.Chassis",
        "#Chassis.1.2.0.Chassis",
        "#Chassis.v1_0_0.Chassis",
        "#Chassis.v1_0_1.Chassis",
        "#Chassis.v1_1_0.Chassis",
        "#Chassis.v1_1_2.Chassis",
        "#Chassis.v1_2_0.Chassis"
})
public class ChassisResourceImpl extends ExternalServiceResourceImpl implements ChassisResource {
    @JsonProperty("ChassisType")
    private ChassisType chassisType;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("SKU")
    private String sku;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("PartNumber")
    private String partNumber;
    @JsonProperty("AssetTag")
    private String assetTag;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Oem")
    private OemResource oem = new OemResource();

    @JsonProperty("Links")
    private Links links = new Links();

    @Override
    public ChassisType getChassisType() {
        return chassisType;
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
    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String getLocationId() {
        if (isNull(oem)) {
            return null;
        }

        RackScaleChassisOemImpl rackScaleChassisOemImpl = oem.getOfType(RackScaleChassisOemImpl.class).orElse(new RackScaleChassisOemImpl());
        return rackScaleChassisOemImpl.location != null ? rackScaleChassisOemImpl.location.getId() : null;
    }

    @Override
    public String getLocationParentId() {
        if (isNull(oem)) {
            return null;
        }

        RackScaleChassisOemImpl rackScaleChassisOemImpl = oem.getOfType(RackScaleChassisOemImpl.class).orElse(new RackScaleChassisOemImpl());
        return rackScaleChassisOemImpl.location != null ? rackScaleChassisOemImpl.location.getParentId() : null;
    }

    @Override
    public RackScaleRackChassisOemImpl getRackChassisAttributes() {
        if (isNull(oem)) {
            return null;
        }

        return oem.getOfType(RackScaleRackChassisOemImpl.class).orElse(new RackScaleRackChassisOemImpl());
    }

    @Override
    @LinkName("powerZones")
    public Iterable<ResourceSupplier> getPowerZones() throws ExternalServiceApiReaderException {
        if (isNull(oem)) {
            return null;
        }

        Optional<RackScaleRackChassisOemImpl> rackChassisOem = oem.getOfType(RackScaleRackChassisOemImpl.class);
        ODataId oDataId = rackChassisOem.isPresent()
                ? rackChassisOem.get().powerZones
                : null;

        return isNull(oDataId) ? null : processMembersListResource(oDataId);
    }

    @Override
    @LinkName("thermalZones")
    public Iterable<ResourceSupplier> getThermalZones() throws ExternalServiceApiReaderException {
        if (isNull(oem)) {
            return null;
        }

        Optional<RackScaleRackChassisOemImpl> rmmOem = oem.getOfType(RackScaleRackChassisOemImpl.class);
        ODataId oDataId = rmmOem.isPresent()
                ? rmmOem.get().thermalZones
                : null;

        return processMembersListResource(oDataId);
    }

    @Override
    @LinkName("containsChassis")
    public Iterable<ResourceSupplier> getContained() throws ExternalServiceApiReaderException {
        return toSuppliers(links.contains);
    }

    @Override
    @LinkName("computerSystems")
    public Iterable<ResourceSupplier> getComputerSystems() throws ExternalServiceApiReaderException {
        return toSuppliers(links.computerSystems);
    }

    @Override
    @LinkName("containsSwitches")
    public Iterable<ResourceSupplier> getSwitches() throws ExternalServiceApiReaderException {
        return toSuppliers(links.switches);
    }

    @OdataTypes("#Intel.Oem.Chassis")
    private static final class RackScaleChassisOemImpl extends OemVendorImpl implements RackScaleChassisOem {
        @JsonProperty("Location")
        private LocationObjectImpl location = new LocationObjectImpl();
    }

    @OdataTypes("http://rackscale.intel.com/schema#Intel.Oem.RackChassis")
    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class RackScaleRackChassisOemImpl extends OemVendorImpl implements RackScaleRackChassisOem {
        @JsonProperty("UUID")
        private UUID uuid;
        @JsonProperty("RackSupportsDisaggregatedPowerCooling")
        private Boolean rackSupportsDisaggregatedPowerCooling;
        @JsonProperty("RackPUID")
        private Integer rackPuid;
        @JsonProperty("GeoTag")
        private String geoTag;
        @JsonProperty("ThermalZones")
        private ODataId thermalZones;
        @JsonProperty("PowerZones")
        private ODataId powerZones;

        @Override
        public UUID getUuid() {
            return uuid;
        }

        @Override
        public Boolean getRackSupportsDisaggregatedPowerCooling() {
            return rackSupportsDisaggregatedPowerCooling;
        }

        @Override
        public Integer getRackPuid() {
            return rackPuid;
        }

        @Override
        public String getGeoTag() {
            return geoTag;
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Links {
        @JsonProperty("Contains")
        private List<ODataId> contains;

        @JsonProperty("ComputerSystems")
        private List<ODataId> computerSystems;

        @JsonProperty("Switches")
        private List<ODataId> switches;

        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;

        @JsonProperty("ManagersInChassis")
        private List<ODataId> managersInChassis;
    }
}
