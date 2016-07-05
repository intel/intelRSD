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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ProcessorDto;
import com.intel.podm.business.dto.redfish.attributes.ProcessorIdDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.properties.ProcessorId;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ProcessorService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PROCESSORS;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ProcessorServiceImpl implements ProcessorService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getProcessorsCollection(Context systemContext) throws EntityNotFoundException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(systemContext);
        return new CollectionDto(PROCESSORS, getAsIdList(system.getProcessors()));
    }

    @Override
    public ProcessorDto getProcessor(Context processorContext) throws EntityNotFoundException {
        Processor processor = (Processor) traverser.traverse(processorContext);
        return ProcessorDto.newBuilder()
                .name(processor.getName())
                .description(processor.getDescription())
                .id(processorContext.getId())
                .socket(processor.getSocket())
                .processorType(processor.getProcessorType())
                .processorArchitecture(processor.getProcessorArchitecture())
                .instructionSet(processor.getInstructionSet())
                .manufacturer(processor.getManufacturer())
                .model(processor.getModel())
                .maxSpeedMhz(processor.getMaxSpeedMhz())
                .totalCores(processor.getTotalCores())
                .totalThreads(processor.getTotalThreads())
                .brand(processor.getBrand())
                .capabilities(processor.getCapabilities())
                .processorId(buildProcessorId(processor.getProcessorId()))
                .status(processor.getStatus())
                .containedBy(processorContext.getParent())
                .build();

    }

    private ProcessorIdDto buildProcessorId(ProcessorId processorId) {
        if (processorId == null) {
            return null;
        }

        return ProcessorIdDto.newBuilder()
                .vendorId(processorId.getVendorId())
                .identificationRegisters(processorId.getIdentificationRegisters())
                .effectiveFamily(processorId.getEffectiveFamily())
                .effectiveModel(processorId.getEffectiveModel())
                .step(processorId.getStep())
                .microcodeInfo(processorId.getMicrocodeInfo())
                .build();
    }
}
