/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.common.enterprise.utils.measures.AmountOfInformation;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MemoryDeviceType;

import java.math.BigDecimal;

import static com.intel.podm.common.enterprise.utils.measures.AmountOfInformation.createAmountOfInformation;
import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.GIBI;
import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.MEBI;

public class MemoryModuleFromMemorySummary implements MemoryModule {
    private AmountOfInformation capacity;

    public MemoryModuleFromMemorySummary(ComputerSystem computerSystem) {
        BigDecimal totalSystemMemoryGiB = computerSystem.getTotalSystemMemoryGiB();
        capacity = totalSystemMemoryGiB != null ? createAmountOfInformation(totalSystemMemoryGiB, GIBI) : null;
    }

    @Override
    public Id getId() {
        return null;
    }

    @Override
    public MemoryDeviceType getMemoryDeviceType() {
        return null;
    }

    @Override
    public Integer getCapacityMib() {
        if (capacity != null) {
            return capacity.getQuantityOf(MEBI).intValue();
        }
        return null;
    }

    @Override
    public Integer getDataWidthBits() {
        return null;
    }

    @Override
    public String getManufacturer() {
        return null;
    }

    @Override
    public Integer getOperatingSpeedMhz() {
        return null;
    }
}
