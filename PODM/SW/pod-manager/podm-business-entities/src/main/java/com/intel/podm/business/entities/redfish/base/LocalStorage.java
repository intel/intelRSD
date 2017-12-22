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

package com.intel.podm.business.entities.redfish.base;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;

import java.math.BigDecimal;
import java.util.Optional;

import static java.util.Optional.empty;

public interface LocalStorage {
    Id getId();
    DiscoverableEntity getParent();
    BigDecimal getCapacityGib();
    MediaType getType();
    BigDecimal getRpm();
    Protocol getProtocol();
    String getSerialNumber();
    boolean needsToBeExplicitlySelected();
    Boolean fromFabricSwitch();
    default Optional<Chassis> getChassis() {
        return empty();
    }
}
