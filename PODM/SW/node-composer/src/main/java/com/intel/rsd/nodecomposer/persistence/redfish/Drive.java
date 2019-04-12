/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.listeners.DriveListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.HasProtocol;
import com.intel.rsd.nodecomposer.types.AttachableType;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.types.AttachableType.DRIVE;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static java.util.Arrays.asList;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "drive")
@EntityListeners(DriveListener.class)
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Drive extends DiscoverableEntity implements HasProtocol, ComposableAsset, AttachableAsset {
    @Getter
    @Setter
    @Column(name = "capacity_bytes")
    private Long capacityBytes;

    @Getter
    @Setter
    @Column(name = "protocol")
    @Enumerated(STRING)
    private Protocol protocol;

    @Getter
    @Setter
    @Column(name = "media_type")
    @Enumerated(STRING)
    private MediaType mediaType;

    @Getter
    @Setter
    @Column(name = "serial_number")
    private String serialNumber;

    @Getter
    @Setter
    @Column(name = "rotation_speed_rpm")
    private BigDecimal rotationSpeedRpm;

    @Getter
    @Setter
    @Column(name = "drive_erased")
    private Boolean driveErased;

    @Column(name = "achievable")
    private Boolean achievable = true;

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST, REMOVE})
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

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "volume_id")
    private Volume volume;

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

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        if (!Objects.equals(this.storageService, storageService)) {
            unlinkStorageService(this.storageService);
            this.storageService = storageService;
            if (storageService != null && !storageService.getDrives().contains(this)) {
                storageService.addDrive(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkDrive(this);
            }
        }
    }

    public Volume getVolume() {
        return volume;
    }

    public void setVolume(Volume volume) {
        this.volume = volume;
    }

    public void unlinkVolume(Volume volume) {
        if (Objects.equals(this.volume, volume)) {
            this.volume = null;
            if (volume != null) {
                volume.unlinkDrives(this);
            }
        }
    }

    @Override
    public boolean isAvailable() {
        if (isProtocolNvme()) {
            return getStatus() != null
                && statusOf(this).isHealthy().verify()
                && (statusOf(this).isEnabled().verify() || statusOf(this).isStandbyOffline().verify());
        }
        return super.isAvailable();
    }

    @Override
    public boolean isDegraded() {
        if (isProtocolNvme()) {
            return getStatus() == null
                || (!statusOf(this).isEnabled().verify() && !statusOf(this).isStandbyOffline().verify())
                || statusOf(this).isCritical().verify();
        }
        return super.isDegraded();
    }

    private boolean isProtocolNvme() {
        return getProtocol() == NVME;
    }

    @Override
    public AttachableType getAttachableType() {
        return DRIVE;
    }

    @Override
    public void preRemove() {
        unlinkChassis(chassis);
        unlinkComposedNode(composedNode);
        unlinkStorage(storage);
        unlinkPcieDeviceFunction(pcieDeviceFunction);
        unlinkStorageService(storageService);
        unlinkVolume(volume);
    }

    @Override
    public boolean isAllocated() {
        return metadata.isAllocated();
    }

    @Override
    public void setAllocated(boolean value) {
        metadata.setAllocated(value);
    }

    @Override
    public Set<Endpoint> getEndpoints() {
        return getEntityConnections().stream()
            .map(ConnectedEntity::getEndpoint)
            .filter(Objects::nonNull)
            .filter(endpoint -> asList(NVME, PCIE).contains(endpoint.getProtocol()))
            .collect(toSet());
    }

    @Override
    public boolean isAchievable() {
        return achievable;
    }

    @Override
    public void setAchievable(boolean achievable) {
        this.achievable = achievable;
    }
}
