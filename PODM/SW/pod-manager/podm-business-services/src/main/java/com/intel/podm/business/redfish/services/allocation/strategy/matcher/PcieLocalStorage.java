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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.Optional;

import static java.lang.Boolean.TRUE;
import static java.util.Objects.hash;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@SuppressWarnings({"checkstyle:MethodCount"})
public class PcieLocalStorage implements LocalStorage {
    private Id id;
    private DiscoverableEntity parent;
    private BigDecimal capacityGib;
    private MediaType type;
    private BigDecimal rpm;
    private Protocol protocol;
    private String serialNumber;
    private Drive drive;
    private Endpoint endpoint;
    private Zone zone;
    private PcieDeviceFunction pcieDeviceFunction;
    private boolean needsToBeExplicitlySelected;
    private Boolean fromFabricSwitch;

    @Override
    public Id getId() {
        return id;
    }

    public void setId(Id id) {
        this.id = id;
    }

    @Override
    public DiscoverableEntity getParent() {
        return parent;
    }

    public void setParent(DiscoverableEntity parent) {
        this.parent = parent;
    }

    @Override
    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    @Override
    public MediaType getType() {
        return type;
    }

    public void setType(MediaType type) {
        this.type = type;
    }

    @Override
    public BigDecimal getRpm() {
        return rpm;
    }

    public void setRpm(BigDecimal rpm) {
        this.rpm = rpm;
    }

    @Override
    public Protocol getProtocol() {
        return protocol;
    }

    public void setProtocol(Protocol protocol) {
        this.protocol = protocol;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public Drive getDrive() {
        return drive;
    }

    public void setDrive(Drive drive) {
        this.drive = drive;
    }

    public Endpoint getEndpoint() {
        return endpoint;
    }

    public void setEndpoint(Endpoint endpoint) {
        this.endpoint = endpoint;
    }

    public Zone getZone() {
        return zone;
    }

    public void setZone(Zone zone) {
        this.zone = zone;
    }

    public PcieDeviceFunction getPcieDeviceFunction() {
        return pcieDeviceFunction;
    }

    public void setPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        this.pcieDeviceFunction = pcieDeviceFunction;
    }

    @Override
    public Boolean fromFabricSwitch() {
        return TRUE;
    }

    @Override
    public boolean needsToBeExplicitlySelected() {
        return needsToBeExplicitlySelected;
    }

    public void setNeedsToBeExplicitlySelected(boolean needsToBeExplicitlySelected) {
        this.needsToBeExplicitlySelected = needsToBeExplicitlySelected;
    }

    @Override
    public Optional<Chassis> getChassis() {
        return getParent() instanceof Chassis
            ? of((Chassis) getParent())
            : empty();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || (!(o instanceof PcieLocalStorage))) {
            return false;
        }
        PcieLocalStorage that = (PcieLocalStorage) o;
        return Objects.equals(getId(), that.getId());
    }

    @Override
    public int hashCode() {
        return hash(id);
    }
}
