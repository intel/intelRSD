/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.embeddables.Fpga;
import com.intel.podm.business.entities.redfish.embeddables.ProcessorId;
import com.intel.podm.client.resources.redfish.ProcessorResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.FpgaMapper;
import com.intel.podm.mappers.subresources.OnPackageMemoryMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static org.apache.commons.lang3.StringUtils.isNumeric;
import static com.intel.podm.common.utils.StringRepresentation.fromMap;

@Dependent
public class ProcessorMapper extends EntityMapper<ProcessorResource, Processor> {
    @Inject
    SimpleTypeMapper simpleTypeMapper;

    @Inject
    OnPackageMemoryMapper onPackageMemoryMapper;

    @Inject
    FpgaMapper fpgaMapper;

    public ProcessorMapper() {
        super(ProcessorResource.class, Processor.class);
        registerProvider(ProcessorId.class, target -> provideProcessorId());
        this.ignoredProperties("extendedIdentificationRegisters");
    }

    @Override
    protected void performNotAutomatedMapping(ProcessorResource sourceProcessor, Processor targetProcessor) {
        super.performNotAutomatedMapping(sourceProcessor, targetProcessor);

        /*
        for backward compatibility reason
        PSME from 1.2 shows only number, while in 2.1 shows "SOCKET X"
         */
        sourceProcessor.getSocket().ifAssigned(socket -> {
            if (isNumeric(socket)) {
                targetProcessor.setSocket("SOCKET " + socket);
            }
        });

        sourceProcessor.getCapabilities().ifAssigned(capabilities ->
            simpleTypeMapper.map(capabilities, targetProcessor.getCapabilities(), targetProcessor::addCapability)
        );

        sourceProcessor.getProcessorMemory().ifAssigned(memoryTypes ->
            onPackageMemoryMapper.map(memoryTypes, targetProcessor.getOnPackageMemory(), targetProcessor::addOnPackageMemory)
        );

        performMappingOnFpga(sourceProcessor, targetProcessor);

        performMappingOnExtendedIdentificationRegisters(sourceProcessor, targetProcessor);
    }

    private void performMappingOnFpga(ProcessorResource sourceProcessor, Processor targetProcessor) {
        sourceProcessor.getFpgaObject().ifAssigned(sourceFpga -> {
            Fpga fpga = targetProcessor.getFpga() == null ? new Fpga() : targetProcessor.getFpga();
            fpgaMapper.map(sourceFpga, fpga);
            targetProcessor.setFpga(fpga);
        });
    }

    private void performMappingOnExtendedIdentificationRegisters(ProcessorResource sourceProcessor, Processor targetProcessor) {
        sourceProcessor.getExtendedIdentificationRegisters().ifAssigned(sourceExtendedIdentificationRegisters ->
                targetProcessor.setExtendedIdentificationRegisters(fromMap(sourceExtendedIdentificationRegisters))
        );
    }

    private ProcessorId provideProcessorId() {
        ProcessorId processorId = target.getProcessorId();
        return processorId == null ? new ProcessorId() : processorId;
    }
}
