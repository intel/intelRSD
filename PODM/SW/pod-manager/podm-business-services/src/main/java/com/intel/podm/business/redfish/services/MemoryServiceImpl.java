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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.MemoryDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ComputerSystemSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.MemoryMerger;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_MODULES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.MEMORY_METRICS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class MemoryServiceImpl implements ReaderService<MemoryDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private ComputerSystemSubResourcesFinder computerSystemSubResourcesFinder;

    @Inject
    private MemoryMerger memoryMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(context);

        // Multi-source resources sanity check
        if (system.isComplementary()) {
            throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
        }

        return new CollectionDto(MEMORY_MODULES, getAsIdSet(computerSystemSubResourcesFinder.getUniqueSubResourcesOfClass(system, Memory.class)));
    }

    @Transactional(REQUIRED)
    @Override
    public MemoryDto getResource(Context context) throws ContextResolvingException {
        Memory memory = (Memory) multiTraverser.traverse(context);
        MemoryDto memoryDto = memoryMerger.toDto(memory);

        if (memory.getMemoryMetrics() != null) {
            memoryDto.setMemoryMetrics(singletonContextOf(context, MEMORY_METRICS_RESOURCE_NAME));
        }

        return memoryDto;
    }
}
