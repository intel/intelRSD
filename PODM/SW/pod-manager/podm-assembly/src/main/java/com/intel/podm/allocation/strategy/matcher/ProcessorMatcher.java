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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.mappers.processor.ProcessorsAllocationMapper;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.dto.redfish.RequestedProcessor;
import com.intel.podm.business.entities.redfish.Processor;

import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.collect.Lists.newArrayList;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

public class ProcessorMatcher {
    public boolean matches(RequestedNode requestedNode, List<Processor> availableProcessors) {
        checkArgument(availableProcessors != null, "Processors collection under computer system cannot be null.");
        return isNotEmpty(requestedNode.getProcessors())
                ?
                areMatched(requestedNode.getProcessors(), availableProcessors)
                :
                true;
    }

    private static boolean areMatched(List<RequestedProcessor> requestedProcessors, List<Processor> availableProcessors) {
        ProcessorsAllocationMapper mapper = new ProcessorsAllocationMapper();
        Map<RequestedProcessor, Processor> mappedProcessors = mapper.map(requestedProcessors, newArrayList(availableProcessors));
        return Objects.equals(mappedProcessors.entrySet().size(), requestedProcessors.size());
    }
}
