/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.dto.StorageServiceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.business.services.context.PathParamConstants.STORAGE_SERVICE_ID;
import static com.intel.podm.business.services.redfish.ReaderService.SERVICE_ROOT_CONTEXT;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@RequestScoped
@Produces(APPLICATION_JSON)
public class StorageServiceCollectionResource extends BaseResource {
    @Inject
    private ReaderService<StorageServiceDto> readerService;

    @Override
    public CollectionDto get() {
        return getOrThrow(() -> readerService.getCollection(SERVICE_ROOT_CONTEXT));
    }

    @Path(STORAGE_SERVICE_ID)
    public StorageServiceResource getStorageService() {
        return getResource(StorageServiceResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder().build();
    }
}
