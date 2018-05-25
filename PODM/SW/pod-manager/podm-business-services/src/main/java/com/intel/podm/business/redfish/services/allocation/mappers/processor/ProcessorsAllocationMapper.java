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

package com.intel.podm.business.redfish.services.allocation.mappers.processor;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.ascendingWhereNullsLast;
import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.contextsAscendingWhereNullsLast;
import static com.intel.podm.business.redfish.services.allocation.mappers.Sorter.sort;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byAtLeastAchievableSpeedMhz;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byAtLeastTotalCores;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byCapabilities;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byExactBrand;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byExactId;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byExactInstructionSet;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byExactModel;
import static com.intel.podm.business.redfish.services.allocation.mappers.processor.Predicates.byProcessorType;
import static com.intel.podm.common.types.EnumeratedType.byPriority;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;

public class ProcessorsAllocationMapper {
    private static Optional<Processor> findMatchingProcessor(RequestedNode.Processor requestedProcessor,
                                                             Collection<Processor> availableProcessors) {
        return availableProcessors.stream()
            .filter(byExactId(requestedProcessor.getResourceContext()))
            .filter(byExactModel(requestedProcessor.getModel()))
            .filter(byExactBrand(requestedProcessor.getBrand()))
            .filter(byAtLeastTotalCores(requestedProcessor.getTotalCores()))
            .filter(byAtLeastAchievableSpeedMhz(requestedProcessor.getAchievableSpeedMhz()))
            .filter(byExactInstructionSet(requestedProcessor.getInstructionSet()))
            .filter(byCapabilities(requestedProcessor.getCapabilities()))
            .filter(byProcessorType(requestedProcessor.getProcessorType()))
            .sorted(getAvailableProcessorsComparator())
            .findFirst();
    }

    private static Comparator<Processor> getAvailableProcessorsComparator() {
        return comparing(Processor::getModel, nullsFirst(naturalOrder()))
            .thenComparing(Processor::getTotalCores, nullsFirst(naturalOrder()))
            .thenComparing(Processor::getMaxSpeedMhz, nullsFirst(naturalOrder()))
            .thenComparing(Processor::getInstructionSet, nullsFirst(byPriority()));
    }

    public Map<RequestedNode.Processor, Processor> map(Collection<RequestedNode.Processor> requestedProcessors,
                                                       Collection<Processor> availableProcessors) {
        Map<RequestedNode.Processor, Processor> mappedProcessors = new HashMap<>();

        Collection<RequestedNode.Processor> requestedProcessorsQueue = sort(requestedProcessors, contextsAscendingWhereNullsLast());
        Collection<Processor> availableProcessorsQueue = sort(availableProcessors, ascendingWhereNullsLast(Processor::getId));

        for (RequestedNode.Processor requestedProcessor : requestedProcessorsQueue) {
            Optional<Processor> matched = findMatchingProcessor(requestedProcessor, availableProcessorsQueue);
            matched.ifPresent(availableProcessor -> mappedProcessors.put(requestedProcessor, availableProcessor));
            matched.ifPresent(availableProcessorsQueue::remove);
        }

        return mappedProcessors;
    }
}
