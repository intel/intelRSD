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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.MemoryChunkDto;
import com.intel.podm.business.dto.redfish.MemoryChunkDto.InterleaveSet;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.MemoryChunk;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.MemoryChunkService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MEMORY_CHUNKS;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class MemoryChunkServiceImpl implements MemoryChunkService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getMemoryChunkCollection(Context systemContext) throws EntityNotFoundException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(systemContext);
        return new CollectionDto(MEMORY_CHUNKS, getAsIdList(computerSystem.getMemoryChunks()));
    }

    @Override
    public MemoryChunkDto getMemoryChunk(Context memoryChunkContext) throws EntityNotFoundException {
        MemoryChunk memoryChunk = (MemoryChunk) traverser.traverse(memoryChunkContext);

        return MemoryChunkDto
                .newBuilder()
                .id(memoryChunk.getId())
                .name(memoryChunk.getName())
                .description(memoryChunk.getDescription())
                .addressRangeType(memoryChunk.getAddressRangeType())
                .interleaveSets(buildInterleaveSets(memoryChunk.getInterleaveSets()))
                .isMirrorEnabled(memoryChunk.getIsMirrorEnabled())
                .isSpare(memoryChunk.getIsSpare())
                .memoryChunkName(memoryChunk.getMemoryChunkName())
                .memoryChunkSizeMib(memoryChunk.getMemoryChunkSizeMib())
                .memoryChunkUid(memoryChunk.getMemoryChunkUid())
                .status(memoryChunk.getStatus())
                .build();
    }

    private List<InterleaveSet> buildInterleaveSets(List<Memory> interleaveSets) {
        return interleaveSets.stream()
                .map(this::mapInterleaveSet)
                .collect(toList());
    }

    private InterleaveSet mapInterleaveSet(Memory memory) {
        return new InterleaveSet(toContext(memory), "0");
    }
}
