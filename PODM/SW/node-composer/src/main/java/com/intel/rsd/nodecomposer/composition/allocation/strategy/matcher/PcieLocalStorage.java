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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.Chassis;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.PcieDeviceFunction;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.Optional;

import static java.lang.Boolean.TRUE;
import static java.util.Objects.hash;
import static java.util.Optional.empty;
import static java.util.Optional.of;

public class PcieLocalStorage implements LocalStorage {
    @Getter
    @Setter
    private ODataId uri;

    @Getter
    @Setter
    private DiscoverableEntity parent;

    @Getter
    @Setter
    private BigDecimal capacityGib;

    @Getter
    @Setter
    private MediaType type;

    @Getter
    @Setter
    private BigDecimal rpm;

    @Getter
    @Setter
    private Protocol protocol;

    @Getter
    @Setter
    private String serialNumber;

    @Getter
    @Setter
    private Drive drive;

    @Getter
    @Setter
    private PcieDeviceFunction pcieDeviceFunction;

    private boolean needsToBeExplicitlySelected;

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
        return Objects.equals(getUri(), that.getUri());
    }

    @Override
    public int hashCode() {
        return hash(uri);
    }
}
