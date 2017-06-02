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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import java.math.BigDecimal;

public class ComputerSystemAttributesMatcher {
    private static final BigDecimal GIB_TO_MIB_RATIO = new BigDecimal(1024);
    public boolean matches(RequestedNode requestedNode, ComputerSystem computerSystem) {
        return matchTotalCores(requestedNode.getTotalSystemCoreCount(), computerSystem)
            && matchTotalMemoryCapacity(requestedNode.getTotalSystemMemoryMib(), computerSystem);
    }

    private boolean matchTotalCores(Integer minTotalCores, ComputerSystem computerSystem) {
        if (minTotalCores == null) {
            return true;
        }

        int computerSystemTotalProcessorCores = computerSystem.getProcessors().stream()
            .filter(cs -> cs.getTotalCores() != null)
            .mapToInt(cs -> cs.getTotalCores())
            .sum();

        return computerSystemTotalProcessorCores >= minTotalCores;
    }

    private boolean matchTotalMemoryCapacity(Integer minCapacityMib, ComputerSystem computerSystem) {
        if (minCapacityMib == null) {
            return true;
        }

        int computerSystemTotalMemoryCapacity;

        if (computerSystem.getMemoryModules().isEmpty() && computerSystem.getTotalSystemMemoryGiB() != null) {
            computerSystemTotalMemoryCapacity = computerSystem.getTotalSystemMemoryGiB().multiply(GIB_TO_MIB_RATIO).intValueExact();
        } else {
            computerSystemTotalMemoryCapacity = computerSystem.getMemoryModules().stream()
                .filter(m -> m.getCapacityMib() != null)
                .mapToInt(m -> m.getCapacityMib())
                .sum();
        }

        return computerSystemTotalMemoryCapacity >= minCapacityMib;
    }
}
