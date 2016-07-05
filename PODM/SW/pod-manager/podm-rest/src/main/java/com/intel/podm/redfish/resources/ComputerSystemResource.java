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

package com.intel.podm.redfish.resources;

import com.intel.podm.actions.ActionException;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.ComputerSystemDto;
import com.intel.podm.business.redfish.services.ComputerSystemBootSourceOverrideService;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ComputerSystemService;
import com.intel.podm.redfish.json.templates.actions.OverrideBootSourceJson;

import javax.inject.Inject;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static com.intel.podm.rest.error.PodmExceptions.resourcesStateMismatch;
import static com.intel.podm.rest.resources.ResourceNames.ADAPTERS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.DEVICES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MEMORY_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MEMORY_CHUNK_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.PROCESSORS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.SIMPLE_STORAGE_RESOURCE_NAME;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class ComputerSystemResource extends BaseResource {
    @Inject
    private ComputerSystemBootSourceOverrideService overrideService;

    @Inject
    private ComputerSystemService computerSystemService;

    @Override
    public ComputerSystemDto get() {
        return getOrThrow(() -> computerSystemService.getComputerSystem(getCurrentContext()));
    }

    @PATCH
    public void overrideBootSource(OverrideBootSourceJson json) {
        validate(json);

        Context context = getCurrentContext();
        try {
            overrideService.overrideBootSource(context, json.boot.bootSourceType, json.boot.bootSourceState);
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (IllegalArgumentException | IllegalStateException e) {
            throw invalidPayload(e.getMessage());
        } catch (ActionException e) {
            throw resourcesStateMismatch("Conflict during overriding boot source", e.getMessage());
        }
    }

    private void validate(OverrideBootSourceJson json) {
        if (json == null || json.boot == null || json.boot.bootSourceType == null || json.boot.bootSourceState == null) {
            throw invalidPayload("Mandatory BootSourceType and/or BootSourceState is missing.");
        }
    }

    @Path(PROCESSORS_RESOURCE_NAME)
    public ProcessorsCollectionResource getProcessorsCollection() {
        return getResource(ProcessorsCollectionResource.class);
    }

    @Path(SIMPLE_STORAGE_RESOURCE_NAME)
    public SimpleStorageCollectionResource getSimpleStorage() {
        return getResource(SimpleStorageCollectionResource.class);
    }

    @Path(ADAPTERS_RESOURCE_NAME)
    public AdaptersCollectionResource getAdaptersCollection() {
        return getResource(AdaptersCollectionResource.class);
    }

    @Path(DEVICES_RESOURCE_NAME)
    public DevicesCollectionResource getDevicesCollection() {
        return getResource(DevicesCollectionResource.class);
    }

    @Path(MEMORY_RESOURCE_NAME)
    public MemoryCollectionResource getMemoryCollection() {
        return getResource(MemoryCollectionResource.class);
    }

    @Path(ETHERNET_INTERFACES_RESOURCE_NAME)
    public EthernetInterfaceCollectionResource getEthernetInterfacesCollection() {
        return getResource(EthernetInterfaceCollectionResource.class);
    }

    @Path(MEMORY_CHUNK_RESOURCE_NAME)
    public MemoryChunkCollectionResource getMemoryChunksCollection() {
        return getResource(MemoryChunkCollectionResource.class);
    }

    @Path("Actions")
    public ComputerSystemActionsResource getComputerSystemActionsResource() {
        return getResource(ComputerSystemActionsResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        return newDefaultOptionsResponseBuilder()
                .addPatchMethod()
                .buildResponse();
    }
}
