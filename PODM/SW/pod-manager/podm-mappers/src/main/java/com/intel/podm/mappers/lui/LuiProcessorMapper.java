/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.mappers.lui;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.properties.ProcessorId;
import com.intel.podm.client.api.resources.redfish.LuiProcessorResource;
import com.intel.podm.client.api.resources.redfish.ProcessorIdObject;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class LuiProcessorMapper extends LuiMapper<LuiProcessorResource, Processor> {
    @Inject
    private GenericDao genericDao;

    public LuiProcessorMapper() {
        super(LuiProcessorResource.class, Processor.class);
        registerProvider(ProcessorId.class, this::provideProcessorId);
    }

    private ProcessorId provideProcessorId(ProcessorIdObject processorIdObject) {
        ProcessorId processorId = target.getProcessorId();

        if (processorId == null) {
            processorId = genericDao.create(ProcessorId.class);
        }

        return processorId;
    }
}
