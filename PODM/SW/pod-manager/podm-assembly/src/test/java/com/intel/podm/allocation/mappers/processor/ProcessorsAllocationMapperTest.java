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

package com.intel.podm.allocation.mappers.processor;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.List;
import java.util.Map;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.InstructionSet.X86;
import static com.intel.podm.common.types.ProcessorBrand.X7;
import static com.intel.podm.templates.assets.ProcessorsCreation.createAvailableProcessor;
import static com.intel.podm.templates.assets.ProcessorsCreation.createRequestedProcessor;
import static java.lang.Integer.valueOf;
import static java.util.Arrays.asList;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodLength"})
public class ProcessorsAllocationMapperTest {
    private ProcessorsAllocationMapper mapper;

    @BeforeClass
    public void setup() {
        this.mapper = new ProcessorsAllocationMapper();
    }

    @Test
    public void whenMappingSingleRequestedWithMultipleAvailable_shouldChooseOneWithTheSmallestSpeedMhz() {
        RequestedNode.Processor requestedWithSpeed500 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 500, X86, 8);
        List<Processor> availableProcessors = asList(
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 504, X86, 12),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 9),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 503, X86, 11),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 502, X86, 10)
        );

        Map<RequestedNode.Processor, Processor> mapped = mapper.map(asList(requestedWithSpeed500), availableProcessors);

        assertEquals(mapped.size(), 1);
        assertEquals(mapped.get(requestedWithSpeed500).getMaxSpeedMhz(), valueOf(501));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldChooseMinimalSubsetBySpeedMhz() {
        RequestedNode.Processor requestedWithSpeed501 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 8);
        RequestedNode.Processor requestedWithSpeed502 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 502, X86, 8);

        List<RequestedNode.Processor> requestedProcessors = asList(requestedWithSpeed501, requestedWithSpeed502);
        List<Processor> availableProcessors = asList(
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 504, X86, 12),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 9),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 503, X86, 11),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 502, X86, 10)
        );

        Map<RequestedNode.Processor, Processor> mapped = mapper.map(requestedProcessors, availableProcessors);

        assertEquals(mapped.size(), 2);
        assertEquals(mapped.get(requestedWithSpeed501).getMaxSpeedMhz(), valueOf(501));
        assertEquals(mapped.get(requestedWithSpeed502).getMaxSpeedMhz(), valueOf(502));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldChooseProperSubsetByIdAndSpeedMhz() {
        RequestedNode.Processor requestedWithSpeed501 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 8);
        RequestedNode.Processor requestedWithSpeed501AndId2 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 8, contextOf(id(2), PROCESSOR));
        RequestedNode.Processor requestedWithSpeed502AndId4 =
                createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 502, X86, 8, contextOf(id(4), PROCESSOR));

        List<RequestedNode.Processor> requestedProcessors = asList(requestedWithSpeed501, requestedWithSpeed501AndId2, requestedWithSpeed502AndId4);
        List<Processor> availableProcessors = asList(
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 504, X86, 12, id(1)),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 501, X86, 9, id(2)),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 503, X86, 11, id(3)),
                createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", X7, 502, X86, 10, id(4))
        );

        Map<RequestedNode.Processor, Processor> mapped = mapper.map(requestedProcessors, availableProcessors);

        assertEquals(mapped.size(), 3);
        assertEquals(mapped.get(requestedWithSpeed501AndId2).getMaxSpeedMhz(), valueOf(501));
        assertEquals(mapped.get(requestedWithSpeed501AndId2).getId(), id(2));

        assertEquals(mapped.get(requestedWithSpeed502AndId4).getMaxSpeedMhz(), valueOf(502));
        assertEquals(mapped.get(requestedWithSpeed502AndId4).getId(), id(4));

        assertEquals(mapped.get(requestedWithSpeed501).getMaxSpeedMhz(), valueOf(503));
        assertEquals(mapped.get(requestedWithSpeed501).getId(), id(3));
    }
}
