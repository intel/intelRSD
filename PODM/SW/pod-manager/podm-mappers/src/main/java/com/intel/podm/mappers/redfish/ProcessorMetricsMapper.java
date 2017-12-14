/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.ProcessorMetrics;
import com.intel.podm.client.resources.redfish.ProcessorMetricsResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class ProcessorMetricsMapper extends EntityMapper<ProcessorMetricsResource, ProcessorMetrics> {
    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    protected ProcessorMetricsMapper() {
        super(ProcessorMetricsResource.class, ProcessorMetrics.class);
    }

    @Override
    protected void performNotAutomatedMapping(ProcessorMetricsResource source, ProcessorMetrics target) {
        super.performNotAutomatedMapping(source, target);

        simpleTypeMapper.map(source.getHealthList(), target.getHealthList(), target::addHealth);
    }
}
