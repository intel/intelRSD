/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.allocation.mappers.memory;

import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.templates.assets.MemoryModulesCreation;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.MemoryDeviceType.DDR3;
import static com.intel.podm.common.types.MemoryDeviceType.SDRAM;
import static java.lang.Integer.valueOf;
import static java.util.Arrays.asList;
import static java.util.stream.Collectors.toList;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodLength"})
public class MemoryModulesAllocationMapperTest {
    private MemoryModulesAllocationMapper mapper;

    @BeforeClass
    public void setup() {
        this.mapper = new MemoryModulesAllocationMapper();
    }

    @Test
    public void whenMappingMultipleRequestedWithUnorderedAvailable_shouldMapWithMinimalCapacityGibSubset() {
        RequestedNode.Memory requestedSdram = MemoryModulesCreation.createRequestedMemory(SDRAM, null, null, new BigDecimal(300), null);
        RequestedNode.Memory requestedDdr3 = MemoryModulesCreation.createRequestedMemory(DDR3, null, null, new BigDecimal(500), null);
        List<RequestedNode.Memory> requested = asList(
                requestedSdram,
                requestedDdr3
        );

        List<Memory> available = asList(
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 152, null),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 200, null),
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 150, null),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 400, null),
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 151, null),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 300, null)
        );

        Map<RequestedNode.Memory, List<MemoryModule>> map = mapper.map(requested, toMemoryModules(available));

        assertEquals(map.size(), 2);
        assertEquals(map.get(requestedSdram).size(), 2);
        assertEquals(map.get(requestedDdr3).size(), 2);
        // minimal subset that satisfies capacityGib constraint for SDRAM:
        assertEquals(map.get(requestedSdram).get(0).getCapacityMib(), valueOf(150));
        assertEquals(map.get(requestedSdram).get(1).getCapacityMib(), valueOf(151));
        // minimal subset that satisfies capacityGib constraint for DDR3:
        assertEquals(map.get(requestedDdr3).get(0).getCapacityMib(), valueOf(200));
        assertEquals(map.get(requestedDdr3).get(1).getCapacityMib(), valueOf(300));
    }

    @Test
    public void whenMappingMultipleRequestedWithUnorderedAvailable_shouldMapWithProperIdAndMinimalCapacityGibSubset() {
        RequestedNode.Memory requestedSdram =
            MemoryModulesCreation.createRequestedMemory(SDRAM, null, null, new BigDecimal(100), null, contextOf(id(5), MEMORY));
        RequestedNode.Memory requestedDdr3 =
            MemoryModulesCreation.createRequestedMemory(DDR3, null, null, new BigDecimal(151), null, contextOf(id(6), MEMORY));
        List<RequestedNode.Memory> requested = asList(
                requestedSdram,
                requestedDdr3
        );

        List<Memory> available = asList(
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 152, null, id(1)),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 200, null, id(2)),
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 150, null, id(3)),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 400, null, id(4)),
                MemoryModulesCreation.createAvailableMemory(SDRAM, null, null, 151, null, id(5)),
                MemoryModulesCreation.createAvailableMemory(DDR3, null, null, 300, null, id(6))
        );

        Map<RequestedNode.Memory, List<MemoryModule>> map = mapper.map(requested, toMemoryModules(available));

        assertEquals(map.size(), 2);
        assertEquals(map.get(requestedSdram).size(), 1);
        assertEquals(map.get(requestedDdr3).size(), 1);
        // minimal subset that satisfies capacityGib constraint for SDRAM:
        assertEquals(map.get(requestedSdram).get(0).getCapacityMib(), valueOf(151));
        assertEquals(map.get(requestedSdram).get(0).getId(), id(5));
        // minimal subset that satisfies capacityGib constraint for DDR3:
        assertEquals(map.get(requestedDdr3).get(0).getCapacityMib(), valueOf(300));
        assertEquals(map.get(requestedDdr3).get(0).getId(), id(6));
    }

    private List<MemoryModule> toMemoryModules(List<Memory> memoryModules) {
        return memoryModules.stream().map(MemoryModule.class::cast).collect(toList());
    }
}
