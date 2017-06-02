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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.mappers.processor.ProcessorsAllocationMapper;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Collections.unmodifiableCollection;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

public class ProcessorMatcher {
    public boolean matches(RequestedNode requestedNode, Collection<Processor> availableProcessors) {
        requires(availableProcessors != null, "Processors collection under computer system cannot be null.");
        List<RequestedNode.Processor> requestedProcessors = requestedNode.getProcessors();

        if (isEmpty(availableProcessors)) {
            return false;
        }

        return isNotEmpty(requestedProcessors)
            ? areMatched(requestedProcessors, availableProcessors)
            : true;
    }

    private static boolean areMatched(List<RequestedNode.Processor> requestedProcessors, Collection<Processor> availableProcessors) {
        ProcessorsAllocationMapper mapper = new ProcessorsAllocationMapper();
        Map<RequestedNode.Processor, Processor> mappedProcessors = mapper.map(requestedProcessors, unmodifiableCollection(availableProcessors));
        return Objects.equals(mappedProcessors.entrySet().size(), requestedProcessors.size());
    }
}
