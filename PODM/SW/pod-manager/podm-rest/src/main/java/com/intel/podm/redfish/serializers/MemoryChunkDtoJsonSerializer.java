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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.MemoryChunkDto;
import com.intel.podm.business.dto.redfish.MemoryChunkDto.InterleaveSet;
import com.intel.podm.redfish.json.templates.MemoryChunkJson;
import com.intel.podm.redfish.json.templates.attributes.InterleaveSetJson;
import com.intel.podm.rest.odataid.ODataContextProvider;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.buildODataId;
import static java.util.stream.Collectors.toList;

public class MemoryChunkDtoJsonSerializer extends DtoJsonSerializer<MemoryChunkDto> {
    protected MemoryChunkDtoJsonSerializer() {
        super(MemoryChunkDto.class);
    }

    @Override
    protected MemoryChunkJson translate(MemoryChunkDto dto) {
        MemoryChunkJson memoryChunkJson = new MemoryChunkJson();

        ODataId oDataId = oDataId(context.getRequestPath());
        memoryChunkJson.oDataId = oDataId;
        memoryChunkJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);

        memoryChunkJson.id = dto.getId();
        memoryChunkJson.name = dto.getName();
        memoryChunkJson.description = dto.getDescription();
        memoryChunkJson.memoryChunkName = dto.getMemoryChunkName();
        memoryChunkJson.memoryChunkSizeMib = dto.getMemoryChunkSizeMib();
        memoryChunkJson.memoryChunkUid = dto.getMemoryChunkUid();
        memoryChunkJson.addressRangeType = dto.getAddressRangeType();
        memoryChunkJson.status = dto.getStatus();
        memoryChunkJson.isMirrorEnabled = dto.isMirrorEnabled();
        memoryChunkJson.isSpare = dto.isSpare();
        memoryChunkJson.interleaveSets = toInterleaveSetJsonCollection(dto.getInterleaveSets());

        return memoryChunkJson;
    }
    public List<InterleaveSetJson> toInterleaveSetJsonCollection(List<InterleaveSet> interleaveSets) {
        return interleaveSets.stream()
                .map(interleaveSet -> new InterleaveSetJson(
                                buildODataId(interleaveSet.getContext()),
                                interleaveSet.getRegionId())
                ).collect(toList());
    }
}
