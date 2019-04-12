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
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import java.math.BigDecimal;
import java.util.Optional;

import static java.lang.Boolean.FALSE;
import static java.util.Optional.empty;
import static java.util.Optional.of;

public class LocalDrive implements LocalStorage {
    @Getter
    @Setter
    private ODataId uri;

    @Getter
    @Setter
    private DiscoverableEntity parent;

    @Getter
    @Setter
    private BigDecimal rpm;

    @Getter
    @Setter
    private BigDecimal capacityGib;

    @Getter
    @Setter
    private MediaType type;

    @Getter
    @Setter
    private Drive drive;

    @Getter
    @Setter
    private String serialNumber;

    @Getter
    @Setter
    private Protocol protocol;

    @Override
    public Boolean fromFabricSwitch() {
        return FALSE;
    }

    @Override
    public boolean needsToBeExplicitlySelected() {
        return false;
    }

    @Override
    public Optional<Chassis> getChassis() {
        return getParent() instanceof Chassis
            ? of((Chassis) getParent())
            : empty();
    }
}
