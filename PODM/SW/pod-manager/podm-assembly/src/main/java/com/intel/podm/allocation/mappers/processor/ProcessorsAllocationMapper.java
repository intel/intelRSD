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

package com.intel.podm.allocation.mappers.processor;

import com.intel.podm.allocation.mappers.Sorter;
import com.intel.podm.business.dto.redfish.RequestedProcessor;
import com.intel.podm.business.entities.redfish.Processor;

import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.google.common.collect.Maps.newHashMap;
import static com.intel.podm.allocation.mappers.processor.Predicates.byAtLeastAchievableSpeedMhz;
import static com.intel.podm.allocation.mappers.processor.Predicates.byAtLeastTotalCores;
import static com.intel.podm.allocation.mappers.processor.Predicates.byCapabilities;
import static com.intel.podm.allocation.mappers.processor.Predicates.byExactBrand;
import static com.intel.podm.allocation.mappers.processor.Predicates.byExactId;
import static com.intel.podm.allocation.mappers.processor.Predicates.byExactInstructionSet;
import static com.intel.podm.allocation.mappers.processor.Predicates.byExactModel;
import static java.util.Comparator.comparing;
import static java.util.Comparator.naturalOrder;
import static java.util.Comparator.nullsFirst;

public class ProcessorsAllocationMapper {
    public Map<RequestedProcessor, Processor> map(List<RequestedProcessor> requestedProcessors, List<Processor> availableProcessors) {
        Map<RequestedProcessor, Processor> mappedProcessors = newHashMap();

        List<RequestedProcessor> requestedProcessorsQueue =
                Sorter.sort(requestedProcessors, Sorter.contextsAscendingWhereNullsLast());
        List<Processor> availableProcessorsQueue =
                Sorter.sort(availableProcessors, Sorter.ascendingWhereNullsLast(Processor::getId));

        for (RequestedProcessor requestedProcessor : requestedProcessorsQueue) {
            Optional<Processor> matched = findMatchingProcessor(requestedProcessor, availableProcessorsQueue);
            matched.ifPresent(availableProcessor -> mappedProcessors.put(requestedProcessor, availableProcessor));
            matched.ifPresent(availableProcessorsQueue::remove);
        }

        return mappedProcessors;
    }

    private static Optional<Processor> findMatchingProcessor(RequestedProcessor requestedProcessor, List<Processor> availableProcessors) {
        return availableProcessors.stream()
                .filter(byExactId(requestedProcessor.getResourceContext()))
                .filter(byExactModel(requestedProcessor.getModel()))
                .filter(byExactBrand(requestedProcessor.getBrand()))
                .filter(byAtLeastTotalCores(requestedProcessor.getTotalCores()))
                .filter(byAtLeastAchievableSpeedMhz(requestedProcessor.getAchievableSpeedMhz()))
                .filter(byExactInstructionSet(requestedProcessor.getInstructionSet()))
                .filter(byCapabilities(requestedProcessor.getCapabilities()))
                .sorted(getAvailableProcessorsComparator())
                .findFirst();
    }

    private static Comparator<Processor> getAvailableProcessorsComparator() {
        return comparing(Processor::getModel, nullsFirst(naturalOrder()))
                .thenComparing(Processor::getTotalCores, nullsFirst(naturalOrder()))
                .thenComparing(Processor::getMaxSpeedMhz, nullsFirst(naturalOrder()))
                .thenComparing(Processor::getInstructionSet, nullsFirst(naturalOrder()));
    }
}
