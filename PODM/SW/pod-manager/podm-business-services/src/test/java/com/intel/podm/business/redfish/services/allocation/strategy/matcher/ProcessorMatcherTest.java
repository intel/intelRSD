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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeWithProcessors;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.List;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.ProcessorsCreation.createAvailableProcessor;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.ProcessorsCreation.createRequestedProcessor;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.InstructionSet.X86;
import static com.intel.podm.common.types.InstructionSet.X86_64;
import static com.intel.podm.common.types.ProcessorBrand.X5;
import static com.intel.podm.common.types.ProcessorBrand.X7;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodCount"})
public class ProcessorMatcherTest {
    private ProcessorMatcher processorMatcher;

    @BeforeClass
    public void setup() {
        this.processorMatcher = new ProcessorMatcher();
    }

    @Test
    public void whenCallingWithNullRequestedAndEmptyAvailable_shouldMatch() {
        RequestedNode requestedNode = new RequestedNodeWithProcessors(null);
        assertFalse(processorMatcher.matches(requestedNode, emptyList()));
    }

    @Test
    public void whenCallingWithSingleRequestedAndEmptyAvailable_shouldNotMatch() {
        RequestedNode requestedNode = new RequestedNodeWithProcessors(
            asList(createRequestedProcessor("Unknown", X7, 150, X86, 2))
        );
        assertFalse(processorMatcher.matches(requestedNode, emptyList()));
    }

    @Test
    public void whenCallingWithSingleRequestedAndSingleAvailable_shouldMatch() {
        shouldMatch(
            createRequestedProcessor("Unknown", X7, 150, X86, 2, contextOf(id(1), PROCESSOR)),
            createAvailableProcessor("Unknown", X7, 150, X86, 2, id(1))
        );
    }

    @Test
    public void whenMoreRequestedThanAvailable_shouldNotMatch() {
        List<Processor> available = asList(createAvailableProcessor("Unknown", X7, 150, X86, 2, id(1)));
        RequestedNode requestedNode = new RequestedNodeWithProcessors(asList(
            createRequestedProcessor("Unknown", X7, 150, X86, 2, contextOf(id(1), PROCESSOR)),
            createRequestedProcessor("Unknown", X7, 150, X86, 2, contextOf(id(2), PROCESSOR))
        ));

        assertFalse(processorMatcher.matches(requestedNode, available));
    }

    @Test
    public void whenMoreAvailableThanRequested_shouldMatch() {
        List<Processor> available = asList(
            createAvailableProcessor("Unknown", X7, 150, X86, 2, id(1)),
            createAvailableProcessor("E5", X7, 150, X86, 2, id(2))
        );
        RequestedNode requestedNode = new RequestedNodeWithProcessors(asList(
            createRequestedProcessor("Unknown", X7, 150, X86, 2, contextOf(id(1), PROCESSOR))
        ));

        assertTrue(processorMatcher.matches(requestedNode, available));
    }

    @Test
    public void whenChangingOrderOrElementsInCollection_shouldMatch() {
        List<Processor> available = asList(
            createAvailableProcessor("Unknown", X7, 56, X86, 2, id(1)),
            createAvailableProcessor("Unknown", X7, 49, X86, 2, id(2)),
            createAvailableProcessor("Unknown", X7, 54, X86, 2, id(3))
        );
        RequestedNode requestedNode1 = new RequestedNodeWithProcessors(asList(
            createRequestedProcessor("Unknown", X7, 48, null, null),
            createRequestedProcessor("Unknown", X7, 55, X86, 2),
            createRequestedProcessor("Unknown", X7, 52, null, 2)
        ));
        RequestedNode requestedNode2 = new RequestedNodeWithProcessors(asList(
            createRequestedProcessor("Unknown", X7, 55, X86, 2),
            createRequestedProcessor("Unknown", X7, 48, null, 2),
            createRequestedProcessor("Unknown", X7, 42, X86, 2)
        ));

        assertTrue(processorMatcher.matches(requestedNode1, available));
        assertTrue(processorMatcher.matches(requestedNode2, available));
    }

    @Test
    public void whenMatchingExactModel_shouldMatch() {
        shouldMatch(
            createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", null, null, null, null),
            createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", null, null, null, null)
        );
    }

    @Test
    public void whenMatchingUnsatisfiedModel_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor("Multi-Core Intel(R) Xeon(R) processor 7xxx Series", null, null, null, null),
            createAvailableProcessor("Multi-Core Intel(R) Xeon(R) processor 5xxx Series", null, null, null, null)
        );
    }

    @Test
    public void whenMatchingExactBrand_shouldMatch() {
        shouldMatch(
            createRequestedProcessor(null, X7, null, null, null),
            createAvailableProcessor(null, X7, null, null, null)
        );
    }

    @Test
    public void whenMatchingUnsatisfiedBrand_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, X7, null, null, null),
            createAvailableProcessor(null, X5, null, null, null)
        );
    }

    @Test
    public void whenMatchingAtLeastTotalCores_shouldMatch() {
        shouldMatch(
            createRequestedProcessor(null, null, null, null, 4),
            createAvailableProcessor(null, null, null, null, 5)
        );
    }

    @Test
    public void whenMatchingUnsatisfiedTotalCores_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, null, null, 6),
            createAvailableProcessor(null, null, null, null, 5)
        );
    }

    @Test
    public void whenAvailableTotalCoresIsNull_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, null, null, 6),
            createAvailableProcessor(null, null, null, null, null)
        );
    }

    @Test
    public void whenAvailableAchievableSpeedMhzIsNull_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, 100, null, null),
            createAvailableProcessor(null, null, null, null, null)
        );
    }

    @Test
    public void whenMatchingAtLeastAchievableSpeedMhz_shouldMatch() {
        shouldMatch(
            createRequestedProcessor(null, null, 100, null, null),
            createAvailableProcessor(null, null, 101, null, null)
        );
    }

    @Test
    public void whenMatchingUnsatisfiedAchievableSpeedMhz_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, 101, null, null),
            createAvailableProcessor(null, null, 100, null, null)
        );
    }

    @Test
    public void whenMatchingExactInstructionSet_shouldMatch() {
        shouldMatch(
            createRequestedProcessor(null, null, null, X86_64, null),
            createAvailableProcessor(null, null, null, X86_64, null)
        );
    }

    @Test
    public void whenMatchingUnsatisfiedInstructionSet_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, null, X86_64, null),
            createAvailableProcessor(null, null, null, X86, null)
        );
    }

    @Test
    public void whenMatchingExactId_shouldMatch() {
        shouldMatch(
            createRequestedProcessor(null, null, null, null, null, contextOf(id(1), PROCESSOR)),
            createAvailableProcessor(null, null, null, null, null, id(1))
        );
    }

    @Test
    public void whenMatchingUnsatisfiedId_shouldNotMatch() {
        shouldNotMatch(
            createRequestedProcessor(null, null, null, null, null, contextOf(id(2), PROCESSOR)),
            createAvailableProcessor(null, null, null, null, null, id(1))
        );
    }

    private void shouldMatch(RequestedNode.Processor requestedProcessor, Processor availableProcessor) {
        RequestedNode requestedNode = new RequestedNodeWithProcessors(asList(requestedProcessor));
        List<Processor> availableMemoryModules = asList(availableProcessor);

        assertEquals(processorMatcher.matches(requestedNode, availableMemoryModules), true);
    }

    private void shouldNotMatch(RequestedNode.Processor requestedProcessor, Processor availableProcessor) {
        RequestedNode requestedNode = new RequestedNodeWithProcessors(asList(requestedProcessor));
        List<Processor> availableMemoryModules = asList(availableProcessor);

        assertEquals(processorMatcher.matches(requestedNode, availableMemoryModules), false);
    }
}
