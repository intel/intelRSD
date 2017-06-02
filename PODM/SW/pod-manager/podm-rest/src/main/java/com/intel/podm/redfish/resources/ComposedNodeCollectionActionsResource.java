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

package com.intel.podm.redfish.resources;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.ViolationsDisclosingException;
import com.intel.podm.business.services.redfish.AllocationService;
import com.intel.podm.redfish.json.templates.assembly.RequestedNodeJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static com.intel.podm.rest.error.PodmExceptions.resourcesStateMismatch;
import static com.intel.podm.rest.error.PodmExceptions.unsupportedCreationRequest;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.net.URI.create;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.created;

@Produces(APPLICATION_JSON)
public class ComposedNodeCollectionActionsResource extends BaseResource {
    private static final String CONFLICT_DURING_ALLOCATION_ERROR_MSG = "Conflict during allocation";

    @Inject
    private AllocationService service;

    @Override
    public Object get() {
        throw notFound();
    }

    @POST
    @Consumes(APPLICATION_JSON)
    @Path("Allocate")
    public Response allocate(RequestedNodeJson requestedNode) {
        try {
            ODataId oDataId = asOdataId(service.allocate(requestedNode));
            return created(create(oDataId.toString())).build();
        } catch (RequestValidationException e) {
            throw unsupportedCreationRequest(e.getViolations());
        } catch (EntityOperationException e) {
            Throwable cause = e.getCause();
            if (cause instanceof ViolationsDisclosingException) {
                ViolationsDisclosingException ex = (ViolationsDisclosingException) cause;
                throw resourcesStateMismatch(CONFLICT_DURING_ALLOCATION_ERROR_MSG, ex.getViolations());
            } else {
                throw resourcesStateMismatch(CONFLICT_DURING_ALLOCATION_ERROR_MSG, e.getMessage());
            }
        }
    }
}
