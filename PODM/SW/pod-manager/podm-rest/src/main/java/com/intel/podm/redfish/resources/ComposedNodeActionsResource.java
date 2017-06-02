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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.AssemblyRequest;
import com.intel.podm.business.services.redfish.requests.AttachEndpointRequest;
import com.intel.podm.business.services.redfish.requests.DetachEndpointRequest;
import com.intel.podm.business.services.redfish.requests.ResetRequest;
import com.intel.podm.redfish.json.templates.actions.AttachEndpointJson;
import com.intel.podm.redfish.json.templates.actions.DetachEndpointJson;
import com.intel.podm.redfish.json.templates.actions.ResetActionJson;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
public class ComposedNodeActionsResource extends BaseResource {
    @Inject
    private ActionService<ResetRequest> resetRequestActionService;

    @Inject
    private ActionService<AssemblyRequest> assemblyRequestActionService;

    @Inject
    private ActionService<AttachEndpointRequest> attachEndpointRequestActionService;

    @Inject
    private ActionService<DetachEndpointRequest> detachEndpointRequestActionService;

    @Override
    public Object get() {
        throw notFound();
    }

    @POST
    @Consumes(APPLICATION_JSON)
    @Path("ComposedNode.Reset")
    public Response reset(ResetActionJson resetActionJson) throws TimeoutException, BusinessApiException {
        resetRequestActionService.perform(getCurrentContext(), resetActionJson);
        return noContent().build();
    }

    @POST
    @Path("ComposedNode.Assemble")
    public Response assemble() throws TimeoutException, BusinessApiException {
        assemblyRequestActionService.perform(getCurrentContext(), null);
        return noContent().build();
    }

    @POST
    @Path("ComposedNode.AttachEndpoint")
    public Response attachEndpoint(AttachEndpointJson attachEndpointJson) throws TimeoutException, BusinessApiException {
        attachEndpointRequestActionService.perform(getCurrentContext(), attachEndpointJson);
        return noContent().build();
    }

    @POST
    @Path("ComposedNode.DetachEndpoint")
    public Response detachEndpoint(DetachEndpointJson detachEndpointJson) throws TimeoutException, BusinessApiException {
        detachEndpointRequestActionService.perform(getCurrentContext(), detachEndpointJson);
        return noContent().build();
    }
}
