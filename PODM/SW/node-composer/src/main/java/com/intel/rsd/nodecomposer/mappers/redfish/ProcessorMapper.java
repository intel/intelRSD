/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.redfish;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ProcessorResource;
import com.intel.rsd.nodecomposer.mappers.EntityMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.SimpleTypeMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class ProcessorMapper extends EntityMapper<ProcessorResource, Processor> {
    private final SimpleTypeMapper simpleTypeMapper;

    @Autowired
    public ProcessorMapper(SimpleTypeMapper simpleTypeMapper) {
        super(ProcessorResource.class, Processor.class);
        this.simpleTypeMapper = simpleTypeMapper;
    }

    @Override
    protected void performNotAutomatedMapping(ProcessorResource sourceProcessor, Processor targetProcessor) {
        super.performNotAutomatedMapping(sourceProcessor, targetProcessor);

        sourceProcessor.getCapabilities().ifAssigned(capabilities ->
            simpleTypeMapper.map(capabilities, targetProcessor.getCapabilities(), targetProcessor::addCapability)
        );

        if (targetProcessor.getProcessorErased() == null) {
            targetProcessor.setProcessorErased(true);
        }
    }
}
