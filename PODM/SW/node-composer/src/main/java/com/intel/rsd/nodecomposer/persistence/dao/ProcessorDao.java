/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.dao;

import com.intel.rsd.nodecomposer.business.dto.redfish.ODataIdPossessor;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Manager;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Switch;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.function.Predicate;
import java.util.stream.Stream;

import static com.intel.rsd.nodecomposer.types.ProcessorType.FPGA;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.SystemType.VIRTUAL;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;

@Slf4j
@Component
public class ProcessorDao extends Dao<Processor> {
    private final PciePortDao pciePortDao;
    private final GenericDao genericDao;
    private final ProcessorHelper processorHelper;

    @Autowired
    public ProcessorDao(PciePortDao pciePortDao, GenericDao genericDao, ProcessorHelper processorHelper) {
        this.pciePortDao = pciePortDao;
        this.genericDao = genericDao;
        this.processorHelper = processorHelper;
    }

    public Set<Processor> getAchievablePcieProcessors(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "ComputerSystem can not be null.");

        List<String> pcieConnectionIds = computerSystem.getPcieConnectionIds();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds).stream()
            .map(this::getAchievablePcieProcessors)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Set<Processor> getAvailableFpgaOverFabricsProcessors() {
        return genericDao.findAll(Processor.class).stream()
            .filter(ProcessorHelper::isRemoteFpga)
            .filter(isFpgaOverFabricProcessor())
            .filter(includeNotAllocatedAndAvailableProcessor())
            .collect(toSet());
    }

    public Set<Processor> retrieveRequestedFpgaProcessors(List<RequestedNode.Processor> processors) {
        return processors.stream()
            .map(ODataIdPossessor::getResourceODataId)
            .filter(Objects::nonNull)
            .map(processorUri -> genericDao.find(Processor.class, processorUri))
            .filter(processor -> VIRTUAL.equals(processor.getComputerSystem().getSystemType()) && FPGA.equals(processor.getProcessorType()))
            .collect(toSet());
    }

    private Set<Processor> getAchievablePcieProcessors(Port upstreamPort) {
        return getConnectedProcessors(upstreamPort)
            .filter(includeNotAllocatedAndAvailableProcessor())
            .collect(toSet());
    }

    private Stream<Processor> getConnectedProcessors(Port upstreamPort) {
        return Stream.of(upstreamPort)
            .map(Port::getSwitch)
            .filter(Objects::nonNull)
            .map(Switch::getFabric)
            .filter(Objects::nonNull)
            .map(Fabric::getManagers)
            .flatMap(Collection::stream)
            .map(Manager::getComputerSystems)
            .flatMap(Collection::stream)
            .map(ComputerSystem::getProcessors)
            .flatMap(Collection::stream);
    }

    private Predicate<Processor> isFpgaOverFabricProcessor() {
        return processor -> processorHelper.processorProtocolEquals(FPGA_OVER_FABRICS, processor);
    }

    private Predicate<Processor> includeNotAllocatedAndAvailableProcessor() {
        return processor -> !processor.isAllocated() && processor.isAvailable();
    }
}
