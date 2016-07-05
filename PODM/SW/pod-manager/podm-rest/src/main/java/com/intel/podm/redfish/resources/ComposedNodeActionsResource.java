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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.services.redfish.ComposedNodeService;
import com.intel.podm.redfish.json.templates.actions.ResetTypeJson;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.rest.error.PodmExceptions.internalServerError;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
public class ComposedNodeActionsResource extends BaseResource {
    @Inject
    private ComposedNodeService service;

    @Override
    public Object get() {
        throw notFound();
    }

    @POST
    @Consumes(APPLICATION_JSON)
    @Path("ComposedNode.Reset")
    public Response reset(ResetTypeJson resetTypeJson) {
        try {
            service.reset(getCurrentContext(), resetTypeJson.resetType);
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (IllegalArgumentException | IllegalStateException e) {
            throw invalidPayload(e.getMessage());
        } catch (EntityOperationException e) {
            throw internalServerError("Reset action could not be completed!", e.getMessage());
        }
        return noContent().build();
    }

    @POST
    @Path("ComposedNode.Assemble")
    public Response assemble() {
        try {
            service.assembleComposedNode(getCurrentContext());
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (EntityOperationException e) {
            throw invalidPayload("Assembly action could not be completed!", e.getMessage());
        }
        return noContent().build();
    }

}
