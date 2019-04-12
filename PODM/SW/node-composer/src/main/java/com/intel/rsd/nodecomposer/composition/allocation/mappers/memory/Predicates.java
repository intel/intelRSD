/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.mappers.memory;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.base.MemoryModule;
import com.intel.rsd.nodecomposer.types.MemoryDeviceType;
import com.intel.rsd.nodecomposer.types.MemoryType;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.function.Predicate;

import static java.util.Objects.isNull;

class Predicates {
    protected Predicates() {
    }

    public static Predicate<? super MemoryModule> byExactUri(ODataId oDataId) {
        return isNull(oDataId) ? alwaysMatches() : memory -> Objects.equals(memory.getUri(), oDataId);
    }

    public static Predicate<? super MemoryModule> byExactMemoryDeviceType(MemoryDeviceType memoryDeviceType) {
        return isNull(memoryDeviceType) ? alwaysMatches() : memory -> Objects.equals(memory.getMemoryDeviceType(), memoryDeviceType);
    }

    public static Predicate<? super MemoryModule> byExactMemoryType(MemoryType memoryType) {
        return isNull(memoryType) ? alwaysMatches() : memory -> Objects.equals(memory.getMemoryType(), memoryType);
    }

    public static Predicate<? super MemoryModule> byAtLeastSpeedMhz(Integer speedMhz) {
        return isNull(speedMhz) ? alwaysMatches() : memory -> {
            Integer availableSpeedMhz = memory.getOperatingSpeedMhz();
            return !isNull(availableSpeedMhz) && availableSpeedMhz >= speedMhz;
        };
    }

    public static Predicate<? super MemoryModule> byExactManufacturer(String manufacturer) {
        return isNull(manufacturer) ? alwaysMatches() : memory -> Objects.equals(memory.getManufacturer(), manufacturer);
    }

    public static Predicate<? super MemoryModule> byAtLeastDataWidthBits(Integer dataWidthBits) {
        return isNull(dataWidthBits) ? alwaysMatches() : memory -> {
            Integer availableDataWidthBits = memory.getDataWidthBits();
            return !isNull(availableDataWidthBits) && availableDataWidthBits >= dataWidthBits;
        };
    }

    public static Predicate<? super MemoryModule> byAtLeastCapacityGib(BigDecimal sizeMiB) {
        return isNull(sizeMiB) ? alwaysMatches() : memory -> {
            Integer availableSizeMiB = memory.getCapacityMib();
            return !isNull(availableSizeMiB) && availableSizeMiB >= sizeMiB.intValue();
        };
    }

    private static Predicate<? super MemoryModule> alwaysMatches() {
        return memory -> true;
    }
}
