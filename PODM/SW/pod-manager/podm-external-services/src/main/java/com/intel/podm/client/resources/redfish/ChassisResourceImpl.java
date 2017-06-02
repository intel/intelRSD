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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.client.resources.redfish.properties.OemVendorImpl;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.client.resources.UnknownOemsHelper.convertObjectNodeToObject;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#Chassis" + OdataTypes.VERSION_PATTERN + "Chassis"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
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
    @JsonProperty("Thermal")
    private ODataId thermal;
    @JsonProperty("Power")
    private ODataId power;
    @JsonProperty("Links")
    private Links links = new Links();
    @JsonProperty("Oem")
    private Oem oem = new Oem();

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
    public Ref<String> getLocationId() {
        if (oem == null) {
            return null;
        }

        RackScaleChassisOemImpl rackScaleChassisOemImpl = oem.getOfType(RackScaleChassisOemImpl.class).orElse(new RackScaleChassisOemImpl());
        return rackScaleChassisOemImpl.location != null ? rackScaleChassisOemImpl.location.getId() : null;
    }

    @Override
    public String getLocationParentId() {
        if (oem == null) {
            return null;
        }

        RackScaleChassisOemImpl rackScaleChassisOemImpl = oem.getOfType(RackScaleChassisOemImpl.class).orElse(new RackScaleChassisOemImpl());
        return rackScaleChassisOemImpl.location != null ? rackScaleChassisOemImpl.location.getParentId() : null;
    }

    @Override
    public RackScaleRackChassisOemImpl getRackChassisAttributes() {
        if (oem == null) {
            return null;
        }

        return oem.getOfType(RackScaleRackChassisOemImpl.class).orElse(null);
    }

    @Override
    @LinkName("thermal")
    public ResourceSupplier getThermal() throws ExternalServiceApiReaderException {
        if (thermal == null) {
            return null;
        }
        return toSupplier(thermal);
    }

    @Override
    @LinkName("powerZones")
    public Iterable<ResourceSupplier> getPowerZones() throws ExternalServiceApiReaderException {
        if (oem == null) {
            return null;
        }

        Optional<RackScaleRackChassisOemImpl> rackChassisOem = oem.getOfType(RackScaleRackChassisOemImpl.class);
        ODataId oDataId = rackChassisOem.isPresent()
            ? rackChassisOem.get().powerZones
            : null;

        return oDataId == null ? null : processMembersListResource(oDataId);
    }

    @Override
    @LinkName("power")
    public ResourceSupplier getPower() {
        if (power == null) {
            return null;
        }

        return toSupplier(power);
    }

    @Override
    @LinkName("thermalZones")
    public Iterable<ResourceSupplier> getThermalZones() throws ExternalServiceApiReaderException {
        if (oem == null) {
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
        return toSuppliers(links.oem.rackScaleOem.switches);
    }

    @Override
    @LinkName("drives")
    public Iterable<ResourceSupplier> getDrives() throws ExternalServiceApiReaderException {
        return toSuppliers(links.drives);
    }

    @Override
    @LinkName("storage")
    public Iterable<ResourceSupplier> getStorage() throws ExternalServiceApiReaderException {
        return toSuppliers(links.storage);
    }

    @OdataTypes("#Intel\\.Oem\\.Chassis")
    private static final class RackScaleChassisOemImpl extends OemVendorImpl implements RackScaleChassisOem {
        @JsonProperty("Location")
        private LocationObjectImpl location = new LocationObjectImpl();
    }

    @OdataTypes({
        "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.RackChassis",
        "#Intel\\.Oem\\.RmmRackChassis"
    })
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
        @JsonProperty("Location")
        private LocationObjectImpl location = new LocationObjectImpl();

        @Override
        public LocationObjectImpl getLocation() {
            return location;
        }

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

    public class Links extends RedfishLinks {
        @JsonProperty("Contains")
        private List<ODataId> contains;

        @JsonProperty("ComputerSystems")
        private List<ODataId> computerSystems;

        @JsonProperty("ManagedBy")
        private List<ODataId> managedBy;

        @JsonProperty("ManagersInChassis")
        private List<ODataId> managersInChassis;

        @JsonProperty("Drives")
        private List<ODataId> drives;

        @JsonProperty("Storage")
        private List<ODataId> storage;

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public class RackScaleOem {
                @JsonProperty("Switches")
                private List<ODataId> switches = new ArrayList<>();
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonIgnore
        private Map<String, OemVendorImpl> vendors = new HashMap<>();

        @Override
        public boolean handleKnownVendor(String oemName, ObjectNode oemValue) throws JsonProcessingException {
            if (oemName.equals("Intel_RackScale")) {
                OemVendorImpl oemVendor = convertObjectNodeToObject(oemValue, OemVendorImpl.class);
                vendors.put(oemName, oemVendor);
                return true;
            }

            return false;
        }

        public <T extends OemVendorImpl> Optional<T> getOfType(Class<T> oemClass) {
            return vendors.values()
                .stream()
                .filter(oemClass::isInstance)
                .map(oemClass::cast)
                .findFirst();
        }
    }
}
