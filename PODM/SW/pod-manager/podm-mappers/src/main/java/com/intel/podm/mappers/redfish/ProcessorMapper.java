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
import com.intel.podm.business.entities.redfish.embeddables.ProcessorId;
import com.intel.podm.client.api.resources.redfish.ProcessorResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static org.apache.commons.lang.StringUtils.isNumeric;

@Dependent
public class ProcessorMapper extends EntityMapper<ProcessorResource, Processor> {
    @Inject
    SimpleTypeMapper simpleTypeMapper;

    public ProcessorMapper() {
        super(ProcessorResource.class, Processor.class);
        registerProvider(ProcessorId.class, target -> provideProcessorId());
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
    }

    private ProcessorId provideProcessorId() {
        ProcessorId processorId = target.getProcessorId();
        return processorId == null ? new ProcessorId() : processorId;
    }
}
