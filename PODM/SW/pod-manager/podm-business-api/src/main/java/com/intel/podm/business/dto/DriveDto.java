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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromSingletonContextHelper.asOdataId;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "indicatorLed", "model", "status",
    "capacityBytes", "protocol", "mediaType", "manufacturer", "serialNumber", "partNumber", "sku", "statusIndicator",
    "revision", "failurePredicted", "assetTag", "capableSpeedGbs", "negotiatedSpeedGbs", "location", "identifiers",
    "hotspareType", "encryptionAbility", "encryptionStatus", "rotationSpeedRpm", "blockSizeBytes", "predictedMediaLifeLeftPercent",
    "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount", "checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength"})
public final class DriveDto extends RedfishDto {
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    private String model;
    private Status status;
    private Long capacityBytes;
    private Protocol protocol;
    private MediaType mediaType;
    private String manufacturer;
    private String serialNumber;
    private String partNumber;
    @JsonProperty("SKU")
    private String sku;
    private StatusIndicator statusIndicator;
    private String revision;
    private Boolean failurePredicted;
    private String assetTag;
    private BigDecimal capableSpeedGbs;
    private BigDecimal negotiatedSpeedGbs;
    private List<LocationDto> location = new ArrayList<>();
    private List<DurableIdentifierDto> identifiers = new ArrayList<>();
    private HotspareType hotspareType;
    private EncryptionAbility encryptionAbility;
    private EncryptionStatus encryptionStatus;
    @JsonProperty("RotationSpeedRPM")
    private BigDecimal rotationSpeedRpm;
    private Integer blockSizeBytes;
    private BigDecimal predictedMediaLifeLeftPercent;
    private Links links = new Links();
    private Actions actions = new Actions();
    private Oem oem = new Oem();

    public DriveDto() {
        super("#Drive.v1_1_1.Drive");
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Long getCapacityBytes() {
        return capacityBytes;
    }

    public void setCapacityBytes(Long capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public Protocol getProtocol() {
        return protocol;
    }

    public void setProtocol(Protocol protocol) {
        this.protocol = protocol;
    }

    public MediaType getMediaType() {
        return mediaType;
    }

    public void setMediaType(MediaType mediaType) {
        this.mediaType = mediaType;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
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

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public StatusIndicator getStatusIndicator() {
        return statusIndicator;
    }

    public void setStatusIndicator(StatusIndicator statusIndicator) {
        this.statusIndicator = statusIndicator;
    }

    public String getRevision() {
        return revision;
    }

    public void setRevision(String revision) {
        this.revision = revision;
    }

    public Boolean getFailurePredicted() {
        return failurePredicted;
    }

    public void setFailurePredicted(Boolean failurePredicted) {
        this.failurePredicted = failurePredicted;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public BigDecimal getCapableSpeedGbs() {
        return capableSpeedGbs;
    }

    public void setCapableSpeedGbs(BigDecimal capableSpeedGbs) {
        this.capableSpeedGbs = capableSpeedGbs;
    }

    public BigDecimal getNegotiatedSpeedGbs() {
        return negotiatedSpeedGbs;
    }

    public void setNegotiatedSpeedGbs(BigDecimal negotiatedSpeedGbs) {
        this.negotiatedSpeedGbs = negotiatedSpeedGbs;
    }

    public List<LocationDto> getLocation() {
        return location;
    }

    public void setLocation(List<LocationDto> location) {
        this.location = location;
    }

    public List<DurableIdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public void setIdentifiers(List<DurableIdentifierDto> identifiers) {
        this.identifiers = identifiers;
    }

    public HotspareType getHotspareType() {
        return hotspareType;
    }

    public void setHotspareType(HotspareType hotspareType) {
        this.hotspareType = hotspareType;
    }

    public EncryptionAbility getEncryptionAbility() {
        return encryptionAbility;
    }

    public void setEncryptionAbility(EncryptionAbility encryptionAbility) {
        this.encryptionAbility = encryptionAbility;
    }

    public EncryptionStatus getEncryptionStatus() {
        return encryptionStatus;
    }

    public void setEncryptionStatus(EncryptionStatus encryptionStatus) {
        this.encryptionStatus = encryptionStatus;
    }

    public BigDecimal getRotationSpeedRpm() {
        return rotationSpeedRpm;
    }

    public void setRotationSpeedRpm(BigDecimal rotationSpeedRpm) {
        this.rotationSpeedRpm = rotationSpeedRpm;
    }

    public Integer getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(Integer blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public BigDecimal getPredictedMediaLifeLeftPercent() {
        return predictedMediaLifeLeftPercent;
    }

    public void setPredictedMediaLifeLeftPercent(BigDecimal predictedMediaLifeLeftPercent) {
        this.predictedMediaLifeLeftPercent = predictedMediaLifeLeftPercent;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    public Actions getActions() {
        return actions;
    }

    public void setActions(Actions actions) {
        this.actions = actions;
    }

    public Oem getOem() {
        return oem;
    }

    public void setOem(Oem oem) {
        this.oem = oem;
    }

    @JsonPropertyOrder({"@odata.type", "volumes", "endpoints", "oem"})
    public class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Drive.v1_1_0.Links";
        @JsonProperty("Volumes")
        private Set<Context> volumes = new LinkedHashSet<>();
        @JsonProperty("Endpoints")
        private Set<Context> endpoints = new LinkedHashSet<>();
        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public Set<Context> getVolumes() {
            return volumes;
        }

        public void setVolumes(Set<Context> volumes) {
            this.volumes = volumes;
        }

        public Set<Context> getEndpoints() {
            return endpoints;
        }

        public void setEndpoints(Set<Context> endpoints) {
            this.endpoints = endpoints;
        }

        public Oem getOem() {
            return oem;
        }

        public void setOem(Oem oem) {
            this.oem = oem;
        }

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemDto {
            public Oem() {
            }
        }
    }

    @JsonPropertyOrder({"secureEraseAction", "oem"})
    public class Actions extends RedfishActionsDto {
        @JsonProperty("#Drive.SecureErase")
        private final SecureEraseAction secureEraseAction = new SecureEraseAction();

        public SecureEraseAction getSecureEraseAction() {
            return secureEraseAction;
        }

        @JsonPropertyOrder({"target"})
        public class SecureEraseAction {
            @JsonProperty("target")
            private String target;

            public String getTarget() {
                return target;
            }

            public void setTarget(SingletonContext target) {
                this.target = asOdataId(target).toString();
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"oDataType", "eraseOnDetach", "firmwareVersion", "driveErased", "pcieFunction", "storage"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Drive";
            private Boolean eraseOnDetach;
            private String firmwareVersion;
            private Boolean driveErased;
            @JsonProperty("PCIeFunction")
            private Context pcieFunction;
            @JsonProperty("Storage")
            private Context storage;

            public String getoDataType() {
                return oDataType;
            }

            public Boolean getEraseOnDetach() {
                return eraseOnDetach;
            }

            public void setEraseOnDetach(Boolean eraseOnDetach) {
                this.eraseOnDetach = eraseOnDetach;
            }

            public String getFirmwareVersion() {
                return firmwareVersion;
            }

            public void setFirmwareVersion(String firmwareVersion) {
                this.firmwareVersion = firmwareVersion;
            }

            public Boolean getDriveErased() {
                return driveErased;
            }

            public void setDriveErased(Boolean driveErased) {
                this.driveErased = driveErased;
            }

            public Context getPcieFunction() {
                return pcieFunction;
            }

            public void setPcieFunction(Context pcieFunction) {
                this.pcieFunction = pcieFunction;
            }

            public Context getStorage() {
                return storage;
            }

            public void setStorage(Context storage) {
                this.storage = storage;
            }
        }
    }
}
