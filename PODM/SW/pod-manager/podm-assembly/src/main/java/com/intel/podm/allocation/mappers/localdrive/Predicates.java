/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.allocation.mappers.localdrive;

import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.StorageControllerInterface;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.function.Predicate;

import static java.util.Objects.isNull;

class Predicates {
    protected Predicates() {
    }

    public static Predicate<? super LocalStorage> byExactId(Context context) {
        return isNull(context) ? alwaysMatches() : localStorage -> {
            //TODO: better way to distinguish?
            if (localStorage instanceof Device) {
                return Objects.equals(localStorage.getId(), context.getId());
            }

            return Objects.equals(localStorage.getParent().getId(), context.getId());
        };
    }

    public static Predicate<? super LocalStorage> byAtLeastCapacityGib(BigDecimal capacityGib) {
        return isNull(capacityGib) ? alwaysMatches() : localStorage -> {
            BigDecimal availableCapacityGib = localStorage.getCapacityGib();
            return !isNull(availableCapacityGib) && availableCapacityGib.compareTo(capacityGib) >= 0;
        };
    }

    public static Predicate<? super LocalStorage> byExactType(DriveType type) {
        return isNull(type) ? alwaysMatches() : localStorage -> Objects.equals(localStorage.getType(), type);
    }

    public static Predicate<? super LocalStorage> byAtLeastMinRpm(Integer minRpm) {
        return isNull(minRpm) ? alwaysMatches() : localStorage -> {
            Integer availableRpm = localStorage.getRpm();
            return !isNull(availableRpm) && availableRpm >= minRpm;
        };
    }

    public static Predicate<? super LocalStorage> byExactSerialNumber(String serialNumber) {
        return isNull(serialNumber) ? alwaysMatches() : localStorage -> Objects.equals(serialNumber, localStorage.getSerialNumber());
    }

    public static Predicate<? super LocalStorage> byExactInterface(StorageControllerInterface storageControllerInterface) {
        return isNull(storageControllerInterface) ? alwaysMatches() : localStorage -> Objects.equals(localStorage.getInterface(), storageControllerInterface);
    }

    private static Predicate<? super LocalStorage> alwaysMatches() {
        return localStorage -> true;
    }
}
