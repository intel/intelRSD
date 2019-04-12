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

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.utils.Converters.convertBytesToGib;
import static java.lang.Boolean.FALSE;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "simple_storage_device")
public class SimpleStorageDevice extends Entity implements LocalStorage {
    @Getter
    @Setter
    @Column(name = "status")
    private Status status;

    @Getter
    @Setter
    @Column(name = "capacity_bytes")
    private BigDecimal capacityBytes;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "simple_storage_id")
    private SimpleStorage simpleStorage;

    //TODO: pzak - what should we have here?
    @Override
    public ODataId getUri() {
        return null;
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
}
