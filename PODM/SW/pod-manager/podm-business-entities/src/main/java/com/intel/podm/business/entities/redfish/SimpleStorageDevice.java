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

import com.intel.podm.business.entities.converters.IdToLongConverter;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import org.hibernate.annotations.Generated;

import javax.persistence.Column;
import javax.persistence.Convert;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Objects;

import static com.intel.podm.common.utils.Converters.convertBytesToGib;
import static java.lang.Boolean.FALSE;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@Table(name = "simple_storage_device", indexes = @Index(name = "idx_simple_storage_device_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class SimpleStorageDevice extends Entity implements LocalStorage {
    @Generated(INSERT)
    @Convert(converter = IdToLongConverter.class)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_NUMERIC_COLUMN_DEFINITION, insertable = false, nullable = false)
    private Id entityId;

    @Column(name = "status")
    private Status status;

    @Column(name = "model")
    private String model;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "name")
    private String name;

    @Column(name = "capacity_bytes")
    private BigDecimal capacityBytes;

    @Column(name = "oem")
    private String oem;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "simple_storage_id")
    private SimpleStorage simpleStorage;

    @Override
    public Id getId() {
        return entityId;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public BigDecimal getCapacityBytes() {
        return capacityBytes;
    }

    public void setCapacityBytes(BigDecimal capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public String getOem() {
        return oem;
    }

    public void setOem(String oem) {
        this.oem = oem;
    }

    public SimpleStorage getSimpleStorage() {
        return simpleStorage;
    }

    public void setSimpleStorage(SimpleStorage simpleStorage) {
        if (!Objects.equals(this.simpleStorage, simpleStorage)) {
            unlinkSimpleStorage(this.simpleStorage);
            this.simpleStorage = simpleStorage;
            if (simpleStorage != null && !simpleStorage.getDevices().contains(this)) {
                simpleStorage.addDevice(this);
            }
        }
    }

    public void unlinkSimpleStorage(SimpleStorage simpleStorage) {
        if (Objects.equals(this.simpleStorage, simpleStorage)) {
            this.simpleStorage = null;
            if (simpleStorage != null) {
                simpleStorage.unlinkDevice(this);
            }
        }
    }

    @Override
    public SimpleStorage getParent() {
        return getSimpleStorage();
    }

    @Override
    public BigDecimal getCapacityGib() {
        return convertBytesToGib(capacityBytes);
    }

    @Override
    public MediaType getType() {
        return null;
    }

    @Override
    public BigDecimal getRpm() {
        return null;
    }

    @Override
    public Protocol getProtocol() {
        return null;
    }

    @Override
    public String getSerialNumber() {
        return null;
    }

    @Override
    public Boolean fromFabricSwitch() {
        return FALSE;
    }

    @Override
    public boolean needsToBeExplicitlySelected() {
        return false;
    }

    @Override
    public void preRemove() {
        unlinkSimpleStorage(simpleStorage);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, simpleStorage);
    }
}
