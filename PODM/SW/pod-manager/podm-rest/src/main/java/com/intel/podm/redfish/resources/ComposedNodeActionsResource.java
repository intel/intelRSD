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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.dto.actions.actionInfo.ActionInfoDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.AssemblyRequest;
import com.intel.podm.business.services.redfish.requests.AttachResourceRequest;
import com.intel.podm.business.services.redfish.requests.DetachResourceRequest;
import com.intel.podm.business.services.redfish.requests.ResetRequest;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.AttachResourceJson;
import com.intel.podm.redfish.json.templates.actions.DetachResourceJson;
import com.intel.podm.redfish.json.templates.actions.ResetActionJson;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.actions.ActionInfoNames.ATTACH_RESOURCE_ACTION_INFO;
import static com.intel.podm.common.types.actions.ActionInfoNames.DETACH_RESOURCE_ACTION_INFO;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceActionBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidHttpMethod;
import static java.lang.String.format;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@RequestScoped
@Produces(APPLICATION_JSON)
public class ComposedNodeActionsResource extends BaseResource {
    @Inject
    private ActionService<ResetRequest> resetRequestActionService;

    @Inject
    private ActionService<AssemblyRequest> assemblyRequestActionService;

    @Inject
    private ActionService<AttachResourceRequest> attachResourceRequestActionService;

    @Inject
    private ActionService<DetachResourceRequest> detachResourceRequestActionService;

    @Override
    public Object get() {
        throw invalidHttpMethod();
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
    @Path("ComposedNode.AttachResource")
    public Response attachEndpoint(AttachResourceJson attachResourceJson) throws TimeoutException, BusinessApiException {
        attachResourceRequestActionService.perform(getCurrentContext(), attachResourceJson);
        return noContent().build();
    }

    @POST
    @Path("ComposedNode.DetachResource")
    public Response detachResource(DetachResourceJson detachResourceJson) throws TimeoutException, BusinessApiException {
        detachResourceRequestActionService.perform(getCurrentContext(), detachResourceJson);
        return noContent().build();
    }

    @GET
    @Path(ATTACH_RESOURCE_ACTION_INFO)
    public RedfishResourceAmazingWrapper getAttachResourceActionInfo() throws BusinessApiException {
        Context context = getCurrentContext();
        ActionInfoDto actionInfoDto = attachResourceRequestActionService.getActionInfo(context);
        return new RedfishResourceAmazingWrapper(singletonContextOf(context, format("Actions/%s", ATTACH_RESOURCE_ACTION_INFO)), actionInfoDto);
    }

    @GET
    @Path(DETACH_RESOURCE_ACTION_INFO)
    public RedfishResourceAmazingWrapper getDetachResourceActionInfo() throws BusinessApiException {
        Context context = getCurrentContext();
        ActionInfoDto actionInfoDto = detachResourceRequestActionService.getActionInfo(context);
        return new RedfishResourceAmazingWrapper(singletonContextOf(context, format("Actions/%s", DETACH_RESOURCE_ACTION_INFO)), actionInfoDto);
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceActionBuilder().build();
    }
}
