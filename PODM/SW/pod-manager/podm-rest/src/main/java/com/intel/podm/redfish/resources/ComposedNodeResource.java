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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ComposedNodeService;
import com.intel.podm.redfish.json.templates.actions.OverrideBootSourceJson;

import javax.inject.Inject;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.internalServerError;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static com.intel.podm.rest.resources.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
public class ComposedNodeResource extends BaseResource {
    @Inject
    private ComposedNodeService service;

    @GET
    @Override
    public ComposedNodeDto get() {
        return getOrThrow(() -> service.getComposedNode(getCurrentContext()));
    }

    @DELETE
    @Override
    public Response delete() {
        try {
            service.deleteComposedNode(getCurrentContext());
            service.terminateComposedNodeTasks(getCurrentContext());
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (EntityOperationException e) {
            throw internalServerError("The specified node could not be disassembled", e.getMessage());
        }

        return noContent().build();
    }

    @PATCH
    public void overrideBootSource(OverrideBootSourceJson json) {
        validate(json);

        Context context = getCurrentContext();
        try {
            service.overrideBootSource(context, json.boot.bootSourceType, json.boot.bootSourceState);
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (IllegalArgumentException | IllegalStateException e) {
            throw invalidPayload(e.getMessage());
        } catch (EntityOperationException e) {
            throw internalServerError("Error during overriding boot source", e.getMessage());
        }
    }

    @Path(COMPOSED_NODES_RESOURCE_NAME)
    public ComposedNodeCollectionResource getComposedNodeCollectionResource() {
        return getResource(ComposedNodeCollectionResource.class);
    }

    @Path("Actions")
    public ComposedNodeActionsResource getComposedNodeActionsResource() {
        return getResource(ComposedNodeActionsResource.class);
    }

    private void validate(OverrideBootSourceJson json) {
        if (json == null || json.boot == null || json.boot.bootSourceType == null || json.boot.bootSourceState == null) {
            throw invalidPayload("Mandatory BootSourceType and/or BootSourceState is missing.");
        }
    }

    @Override
    protected Response createOptionsResponse() {
        return newDefaultOptionsResponseBuilder()
                .addDeleteMethod()
                .addPatchMethod()
                .buildResponse();
    }
}
