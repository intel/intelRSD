/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.mappers.localdrive;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.SimpleStorageDevice;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

import static java.util.Objects.isNull;

class Predicates {
    protected Predicates() {
    }

    public static Predicate<LocalStorage> byExactId(Context context) {
        return isNull(context)
                ?
                localStorage -> !localStorage.needsToBeExplicitlySelected()
                :
                localStorage -> {
                    //TODO: RSASW-8090
                    if (localStorage instanceof SimpleStorageDevice) {
                        return Objects.equals(localStorage.getParent().getId(), context.getId());
                    }

                    return Objects.equals(localStorage.getId(), context.getId());
                };
    }

    public static Predicate<LocalStorage> byAtLeastCapacityGib(BigDecimal capacityGib) {
        return isNull(capacityGib) ? alwaysMatches() : localStorage -> {
            BigDecimal availableCapacityGib = localStorage.getCapacityGib();
            return !isNull(availableCapacityGib) && availableCapacityGib.compareTo(capacityGib) >= 0;
        };
    }

    public static Predicate<LocalStorage> byExactType(MediaType type) {
        return isNull(type) ? alwaysMatches() : localStorage -> Objects.equals(localStorage.getType(), type);
    }

    public static Predicate<LocalStorage> byChassisContext(Context chassisContext) {
        return isNull(chassisContext)
            ? alwaysMatches()
            : localStorage -> {
                Optional<Chassis> chassis = localStorage.getChassis();
                return Objects.equals(chassisContext.getId(), chassis.map(Chassis::getId).orElse(null));
            };
    }

    public static Predicate<LocalStorage> byAtLeastMinRpm(BigDecimal minRpm) {
        return isNull(minRpm) ? alwaysMatches() : localStorage -> {
            BigDecimal availableRpm = localStorage.getRpm();
            return !isNull(availableRpm) && availableRpm.compareTo(minRpm) >= 0;
        };
    }

    public static Predicate<LocalStorage> byExactSerialNumber(String serialNumber) {
        return isNull(serialNumber) ? alwaysMatches() : localStorage -> Objects.equals(serialNumber, localStorage.getSerialNumber());
    }

    public static Predicate<LocalStorage> byExactInterface(Protocol protocol) {
        return isNull(protocol) ? alwaysMatches()
                : localStorage -> Objects.equals(localStorage.getProtocol(), protocol);
    }

    public static Predicate<LocalStorage> isFromFabricSwitch(Boolean isFromFabricSwitch) {
        return isNull(isFromFabricSwitch) ? alwaysMatches()
            : localStorage -> Objects.equals(localStorage.fromFabricSwitch(), isFromFabricSwitch);
    }

    private static Predicate<LocalStorage> alwaysMatches() {
        return localStorage -> true;
    }
}
