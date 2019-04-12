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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.mappers.processor.ProcessorsAllocationMapper;
import com.intel.rsd.nodecomposer.persistence.dao.ProcessorDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static java.util.Collections.unmodifiableCollection;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ProcessorMatcher {
    private final ProcessorsAllocationMapper mapper;
    private final ProcessorDao processorDao;

    @Autowired
    public ProcessorMatcher(ProcessorsAllocationMapper mapper, ProcessorDao processorDao) {
        this.mapper = mapper;
        this.processorDao = processorDao;
    }

    public boolean matches(RequestedNode requestedNode, ComputerSystem computerSystem) {
        Set<Processor> processors = computerSystem.getProcessors();
        requires(processors != null, "Processors collection under computer system cannot be null.");
        Collection<Processor> availableProcessors = new HashSet<>(processors);

        if (isEmpty(availableProcessors)) {
            return false;
        }
        availableProcessors.addAll(getAchievableAndAvailablePcieProcessors(computerSystem));
        availableProcessors.addAll(getAvailableFpgaOverFabricsProcessors());

        List<RequestedNode.Processor> requestedProcessors = requestedNode.getProcessors();
        return isEmpty(requestedProcessors) || areMatched(requestedProcessors, getAchievableProcessors(availableProcessors));
    }

    private Set<Processor> getAchievableProcessors(Collection<Processor> availableProcessors) {
        return availableProcessors.stream()
            .filter(Processor::isAchievable)
            .collect(toSet());
    }

    private boolean areMatched(List<RequestedNode.Processor> requestedProcessors, Collection<Processor> availableProcessors) {
        Map<RequestedNode.Processor, Processor> mappedProcessors = mapper.map(requestedProcessors, unmodifiableCollection(availableProcessors));
        return Objects.equals(mappedProcessors.entrySet().size(), requestedProcessors.size());
    }

    private Collection<Processor> getAchievableAndAvailablePcieProcessors(ComputerSystem computerSystem) {
        return processorDao.getAchievablePcieProcessors(computerSystem).stream()
            .filter(processor -> processor.getEndpoints().isEmpty() || processor.getEndpoints().stream().anyMatch(Endpoint::isAttachable))
            .collect(toList());
    }

    private Collection<Processor> getAvailableFpgaOverFabricsProcessors() {
        return processorDao.getAvailableFpgaOverFabricsProcessors();
    }
}
