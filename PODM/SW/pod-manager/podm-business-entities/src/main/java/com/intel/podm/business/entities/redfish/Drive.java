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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.listeners.DriveListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.entities.redfish.embeddables.RedfishLocation;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.StatusIndicator;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.types.State.STANDBY_OFFLINE;
import static java.util.stream.Stream.of;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "drive", indexes = @Index(name = "idx_drive_entity_id", columnList = "entity_id", unique = true))
@EntityListeners(DriveListener.class)
@NamedQueries({
    @NamedQuery(name = Drive.GET_PRIMARY_DRIVE,
        query = "SELECT drive "
            + "FROM Drive drive "
            + "WHERE drive.multiSourceDiscriminator = :multiSourceDiscriminator "
            + "AND drive.isComplementary = false"
    )
})
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Drive extends DiscoverableEntity implements MultiSourceResource {
    public static final String GET_PRIMARY_DRIVE = "GET_PRIMARY_DRIVE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "status_indicator")
    @Enumerated(STRING)
    private StatusIndicator statusIndicator;

    @Column(name = "indicator_led")
    @Enumerated(STRING)
    private IndicatorLed indicatorLed;

    @Column(name = "model")
    private String model;

    @Column(name = "revision")
    private String revision;

    @Column(name = "capacity_bytes")
    private Long capacityBytes;

    @Column(name = "failure_predicted")
    private Boolean failurePredicted;

    @Column(name = "protocol")
    @Enumerated(STRING)
    private Protocol protocol;

    @Column(name = "media_type")
    @Enumerated(STRING)
    private MediaType mediaType;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "sku")
    private String sku;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "asset_tag")
    private String assetTag;

    @Column(name = "hotspare_type")
    @Enumerated(STRING)
    private HotspareType hotspareType;

    @Column(name = "encryption_ability")
    @Enumerated(STRING)
    private EncryptionAbility encryptionAbility;

    @Column(name = "encryption_status")
    @Enumerated(STRING)
    private EncryptionStatus encryptionStatus;

    @Column(name = "rotation_speed_rpm")
    private BigDecimal rotationSpeedRpm;

    @Column(name = "block_size_bytes")
    private Integer blockSizeBytes;

    @Column(name = "capable_speed_gbs")
    private BigDecimal capableSpeedGbs;

    @Column(name = "negotiated_speed_gbs")
    private BigDecimal negotiatedSpeedGbs;

    @Column(name = "predicted_media_life_left_percent")
    private BigDecimal predictedMediaLifeLeftPercent;

    @Column(name = "erase_on_detach")
    private Boolean eraseOnDetach;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @Column(name = "drive_erased")
    private Boolean driveErased;

    @Column(name = "multi_source_discriminator")
    private String multiSourceDiscriminator;

    @ElementCollection
    @CollectionTable(name = "drive_location", joinColumns = @JoinColumn(name = "drive_id"))
    @OrderColumn(name = "location_order")
    private List<RedfishLocation> locations = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "drive_identifier", joinColumns = @JoinColumn(name = "drive_id"))
    @OrderColumn(name = "identifier_order")
    private List<Identifier> identifiers = new ArrayList<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "drive_metadata_id")
    private DriveMetadata metadata = new DriveMetadata();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_id")
    private Storage storage;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "pcie_device_function_id")
    private PcieDeviceFunction pcieDeviceFunction;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public StatusIndicator getStatusIndicator() {
        return statusIndicator;
    }

    public void setStatusIndicator(StatusIndicator statusIndicator) {
        this.statusIndicator = statusIndicator;
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

    public String getRevision() {
        return revision;
    }

    public void setRevision(String revision) {
        this.revision = revision;
    }

    public Long getCapacityBytes() {
        return capacityBytes;
    }

    public void setCapacityBytes(Long capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public Boolean getFailurePredicted() {
        return failurePredicted;
    }

    public void setFailurePredicted(Boolean failurePredicted) {
        this.failurePredicted = failurePredicted;
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

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
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

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
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

    public BigDecimal getPredictedMediaLifeLeftPercent() {
        return predictedMediaLifeLeftPercent;
    }

    public void setPredictedMediaLifeLeftPercent(BigDecimal predictedMediaLifeLeftPercent) {
        this.predictedMediaLifeLeftPercent = predictedMediaLifeLeftPercent;
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

    @Override
    public String getMultiSourceDiscriminator() {
        return multiSourceDiscriminator;
    }

    @Override
    public void setMultiSourceDiscriminator(String multiSourceDiscriminator) {
        this.multiSourceDiscriminator = multiSourceDiscriminator;
    }

    public List<RedfishLocation> getLocation() {
        return locations;
    }

    public void addLocation(RedfishLocation location) {
        locations.add(location);
    }

    public List<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public DriveMetadata getMetadata() {
        return metadata;
    }

    public Chassis getChassis() {
        return chassis;
    }

    public void setChassis(Chassis chassis) {
        if (!Objects.equals(this.chassis, chassis)) {
            unlinkChassis(this.chassis);
            this.chassis = chassis;
            if (chassis != null && !chassis.getDrives().contains(this)) {
                chassis.addDrive(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkDrive(this);
            }
        }
    }

    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !composedNode.getDrives().contains(this)) {
                composedNode.addDrive(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkDrive(this);
            }
        }
    }

    public Storage getStorage() {
        return storage;
    }

    public void setStorage(Storage storage) {
        if (!Objects.equals(this.storage, storage)) {
            unlinkStorage(this.storage);
            this.storage = storage;
            if (storage != null && !storage.getDrives().contains(this)) {
                storage.addDrive(this);
            }
        }
    }

    public void unlinkStorage(Storage storage) {
        if (Objects.equals(this.storage, storage)) {
            this.storage = null;
            if (storage != null) {
                storage.unlinkDrive(this);
            }
        }
    }

    public PcieDeviceFunction getPcieDeviceFunction() {
        return pcieDeviceFunction;
    }

    public void setPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (!Objects.equals(this.pcieDeviceFunction, pcieDeviceFunction)) {
            unlinkPcieDeviceFunction(this.pcieDeviceFunction);
            this.pcieDeviceFunction = pcieDeviceFunction;
            if (pcieDeviceFunction != null && !pcieDeviceFunction.getDrives().contains(this)) {
                pcieDeviceFunction.addDrive(this);
            }
        }
    }

    public void unlinkPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (Objects.equals(this.pcieDeviceFunction, pcieDeviceFunction)) {
            this.pcieDeviceFunction = null;
            if (pcieDeviceFunction != null) {
                pcieDeviceFunction.unlinkDrive(this);
            }
        }
    }

    @Override
    public boolean canBeAllocated() {
        if (getProtocol() == NVME) {
            return !getMetadata().isAllocated()
                && getStatus() != null
                && Objects.equals(getStatus().getHealth(), OK)
                && of(ENABLED, STANDBY_OFFLINE).anyMatch(state -> state == getStatus().getState());
        }
        return super.canBeAllocated();
    }

    @Override
    public void preRemove() {
        unlinkChassis(chassis);
        unlinkComposedNode(composedNode);
        unlinkStorage(storage);
        unlinkPcieDeviceFunction(pcieDeviceFunction);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
