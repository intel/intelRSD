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

package com.intel.podm.redfish.resources;

import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.redfish.MemoryChunkService;
import com.intel.podm.rest.resources.PathParamConstants;

import javax.inject.Inject;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class MemoryChunkCollectionResource extends BaseResource {

    @Inject
    private MemoryChunkService memoryChunkService;

    @Override
    public CollectionDto get() {
        return getOrThrow(() -> memoryChunkService.getMemoryChunkCollection(getCurrentContext()));
    }

    @Path(PathParamConstants.MEMORY_CHUNK_ID)
    public MemoryChunkResource getMemoryChunk() {
        return getResource(MemoryChunkResource.class);
    }
}
