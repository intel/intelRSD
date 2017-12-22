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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.StorageControllerInterface;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "physical_drive", indexes = @Index(name = "idx_physical_drive_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class PhysicalDrive extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "storage_controller_interface")
    @Enumerated(STRING)
    private StorageControllerInterface storageControllerInterface;

    @Column(name = "capacity_gib")
    private BigDecimal capacityGib;

    @Column(name = "type")
    @Enumerated(STRING)
    private MediaType type;

    @Column(name = "rpm")
    private Integer rpm;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @Column(name = "serial_number")
    private String serialNumber;

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "physical_drive_logical_drive",
        joinColumns = {@JoinColumn(name = "physical_drive_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "logical_drive_id", referencedColumnName = "id")})
    private Set<LogicalDrive> logicalDrives = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public StorageControllerInterface getControllerInterface() {
        return storageControllerInterface;
    }

    public void setControllerInterface(StorageControllerInterface storageControllerInterface) {
        this.storageControllerInterface = storageControllerInterface;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    public MediaType getType() {
        return type;
    }

    public void setType(MediaType type) {
        this.type = type;
    }

    public Integer getRpm() {
        return rpm;
    }

    public void setRpm(Integer rpm) {
        this.rpm = rpm;
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

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public Set<LogicalDrive> getLogicalDrives() {
        return logicalDrives;
    }

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        requiresNonNull(logicalDrive, "logicalDrive");

        logicalDrives.add(logicalDrive);
        if (!logicalDrive.getPhysicalDrives().contains(this)) {
            logicalDrive.addPhysicalDrive(this);
        }
    }

    public void unlinkLogicalDrive(LogicalDrive logicalDrive) {
        if (logicalDrives.contains(logicalDrive)) {
            logicalDrives.remove(logicalDrive);
            if (logicalDrive != null) {
                logicalDrive.unlinkPhysicalDrive(this);
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
            if (storageService != null && !storageService.getPhysicalDrives().contains(this)) {
                storageService.addPhysicalDrive(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkPhysicalDrive(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(logicalDrives, this::unlinkLogicalDrive);
        unlinkStorageService(storageService);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, storageService);
    }
}
