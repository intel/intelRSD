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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;
import com.intel.podm.common.types.redfish.DurableIdentifier;
import com.intel.podm.common.types.redfish.RedfishDrive;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.math.BigDecimal;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount", "checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength"})
public final class DriveDto extends BaseDto implements RedfishDrive {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final StatusIndicator statusIndicator;
    private final IndicatorLed indicatorLed;
    private final String model;
    private final String revision;
    private final Long capacityBytes;
    private final Boolean failurePredicted;
    private final Protocol protocol;
    private final MediaType mediaType;
    private final String manufacturer;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final String assetTag;
    private final List<DurableIdentifier> identifiers;
    private final List<RedfishDrive.Location> location;
    private final HotspareType hotspareType;
    private final EncryptionAbility encryptionAbility;
    private final EncryptionStatus encryptionStatus;
    private final BigDecimal rotationSpeedRpm;
    private final Integer blockSizeBytes;
    private final BigDecimal capableSpeedGbs;
    private final BigDecimal negotiatedSpeedGbs;
    private final BigDecimal predictedMediaLifeLeftPercent;
    private final Boolean eraseOnDetach;
    private final String firmwareVersion;
    private final Boolean driveErased;
    private final Status status;
    private final Context pcieFunction;
    private final Context storage;
    private final Links links;

    private DriveDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        statusIndicator = builder.statusIndicator;
        indicatorLed = builder.indicatorLed;
        model = builder.model;
        revision = builder.revision;
        capacityBytes = builder.capacityBytes;
        failurePredicted = builder.failurePredicted;
        protocol = builder.protocol;
        mediaType = builder.mediaType;
        manufacturer = builder.manufacturer;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        assetTag = builder.assetTag;
        identifiers = builder.identifiers;
        location = builder.location;
        hotspareType = builder.hotspareType;
        encryptionAbility = builder.encryptionAbility;
        encryptionStatus = builder.encryptionStatus;
        rotationSpeedRpm = builder.rotationSpeedRpm;
        blockSizeBytes = builder.blockSizeBytes;
        capableSpeedGbs = builder.capableSpeedGbs;
        negotiatedSpeedGbs = builder.negotiatedSpeedGbs;
        predictedMediaLifeLeftPercent = builder.predictedMediaLifeLeftPercent;
        eraseOnDetach = builder.eraseOnDetach;
        firmwareVersion = builder.firmwareVersion;
        driveErased = builder.driveErased;
        status = builder.status;
        pcieFunction = builder.pcieFunction;
        storage = builder.storage;
        links = new Links(builder.endpoints, builder.volumes);
    }

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
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    @Override
    public StatusIndicator getStatusIndicator() {
        return statusIndicator;
    }

    @Override
    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getRevision() {
        return revision;
    }

    @Override
    public Long getCapacityBytes() {
        return capacityBytes;
    }

    @Override
    public Boolean getFailurePredicted() {
        return failurePredicted;
    }

    @Override
    public Protocol getProtocol() {
        return protocol;
    }

    @Override
    public MediaType getMediaType() {
        return mediaType;
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
    public List<DurableIdentifier> getIdentifiers() {
        return identifiers;
    }

    @Override
    public List<RedfishDrive.Location> getLocation() {
        return location;
    }

    @Override
    public HotspareType getHotspareType() {
        return hotspareType;
    }

    @Override
    public EncryptionAbility getEncryptionAbility() {
        return encryptionAbility;
    }

    @Override
    public EncryptionStatus getEncryptionStatus() {
        return encryptionStatus;
    }

    @Override
    public BigDecimal getRotationSpeedRpm() {
        return rotationSpeedRpm;
    }

    @Override
    public Integer getBlockSizeBytes() {
        return blockSizeBytes;
    }

    @Override
    public BigDecimal getCapableSpeedGbs() {
        return capableSpeedGbs;
    }

    @Override
    public BigDecimal getNegotiatedSpeedGbs() {
        return negotiatedSpeedGbs;
    }

    @Override
    public BigDecimal getPredictedMediaLifeLeftPercent() {
        return predictedMediaLifeLeftPercent;
    }

    @Override
    public Boolean getEraseOnDetach() {
        return eraseOnDetach;
    }

    @Override
    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    @Override
    public Boolean getDriveErased() {
        return driveErased;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public Context getPcieFunction() {
        return pcieFunction;
    }

    public Context getStorage() {
        return storage;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> volumes;
        private final Set<Context> endpoints;

        public Links(Set<Context> endpoints, Set<Context> volumes) {
            this.volumes = volumes;
            this.endpoints = endpoints;
        }

        public Set<Context> getVolumes() {
            return volumes;
        }

        public Set<Context> getEndpoints() {
            return endpoints;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private StatusIndicator statusIndicator;
        private IndicatorLed indicatorLed;
        private String model;
        private String revision;
        private Long capacityBytes;
        private Boolean failurePredicted;
        private Protocol protocol;
        private MediaType mediaType;
        private String manufacturer;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private String assetTag;
        private List<DurableIdentifier> identifiers;
        private List<RedfishDrive.Location> location;
        private HotspareType hotspareType;
        private EncryptionAbility encryptionAbility;
        private EncryptionStatus encryptionStatus;
        private BigDecimal rotationSpeedRpm;
        private Integer blockSizeBytes;
        private BigDecimal capableSpeedGbs;
        private BigDecimal negotiatedSpeedGbs;
        private BigDecimal predictedMediaLifeLeftPercent;
        private Boolean eraseOnDetach;
        private String firmwareVersion;
        private Boolean driveErased;
        private Status status;
        private Set<Context> endpoints = new LinkedHashSet<>();
        private Set<Context> volumes = new LinkedHashSet<>();
        private Context pcieFunction;
        private Context storage;

        private Builder() {
        }

        public DriveDto build() {
            return new DriveDto(this);
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder statusIndicator(StatusIndicator statusIndicator) {
            this.statusIndicator = statusIndicator;
            return this;
        }

        public Builder indicatorLed(IndicatorLed indicatorLed) {
            this.indicatorLed = indicatorLed;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder revision(String revision) {
            this.revision = revision;
            return this;
        }

        public Builder capacityBytes(Long capacityBytes) {
            this.capacityBytes = capacityBytes;
            return this;
        }

        public Builder failurePredicted(Boolean failurePredicted) {
            this.failurePredicted = failurePredicted;
            return this;
        }

        public Builder protocol(Protocol protocol) {
            this.protocol = protocol;
            return this;
        }

        public Builder mediaType(MediaType mediaType) {
            this.mediaType = mediaType;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder sku(String sku) {
            this.sku = sku;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public Builder identifiers(List<DurableIdentifier> identifiers) {
            this.identifiers = identifiers;
            return this;
        }

        public Builder location(List<Location> location) {
            this.location = location;
            return this;
        }

        public Builder hotspareType(HotspareType hotspareType) {
            this.hotspareType = hotspareType;
            return this;
        }

        public Builder encryptionAbility(EncryptionAbility encryptionAbility) {
            this.encryptionAbility = encryptionAbility;
            return this;
        }

        public Builder encryptionStatus(EncryptionStatus encryptionStatus) {
            this.encryptionStatus = encryptionStatus;
            return this;
        }

        public Builder rotationSpeedRpm(BigDecimal rotationSpeedRpm) {
            this.rotationSpeedRpm = rotationSpeedRpm;
            return this;
        }

        public Builder blockSizeBytes(Integer blockSizeBytes) {
            this.blockSizeBytes = blockSizeBytes;
            return this;
        }

        public Builder capableSpeedGbs(BigDecimal capableSpeedGbs) {
            this.capableSpeedGbs = capableSpeedGbs;
            return this;
        }

        public Builder negotiatedSpeedGbs(BigDecimal negotiatedSpeedGbs) {
            this.negotiatedSpeedGbs = negotiatedSpeedGbs;
            return this;
        }

        public Builder predictedMediaLifeLeftPercent(BigDecimal predictedMediaLifeLeftPercent) {
            this.predictedMediaLifeLeftPercent = predictedMediaLifeLeftPercent;
            return this;
        }

        public Builder eraseOnDetach(Boolean eraseOnDetach) {
            this.eraseOnDetach = eraseOnDetach;
            return this;
        }

        public Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
            return this;
        }

        public Builder driveErased(Boolean driveErased) {
            this.driveErased = driveErased;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder endpoints(Context endpoint) {
            this.endpoints.add(endpoint);
            return this;
        }

        public Builder pcieFunction(Context pcieFunction) {
            this.pcieFunction = pcieFunction;
            return this;
        }

        public Builder storage(Context storage) {
            this.storage = storage;
            return this;
        }

        public Builder volumes(Context volume) {
            this.volumes.add(volume);
            return this;
        }
    }
}
