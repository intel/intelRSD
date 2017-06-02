/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EventSubscriptionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.requests.EventSubscriptionRequest;
import com.intel.podm.redfish.json.templates.actions.EventSubscriptionRequestJson;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.business.services.redfish.ReaderService.SERVICE_ROOT_CONTEXT;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.common.types.Id.id;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EventSubscriptionCollectionResource extends BaseResource {
    @Inject
    private ReaderService<EventSubscriptionDto> readerService;
    @Inject
    private CreationService<EventSubscriptionRequest> creationService;
    @Inject
    private RemovalService<EventSubscriptionRequest> removalService;

    @GET
    @Override
    public CollectionDto get() {
        return getOrThrow(() -> readerService.getCollection(SERVICE_ROOT_CONTEXT));
    }

    @POST
    @Consumes(APPLICATION_JSON)
    public Response createSubscription(EventSubscriptionRequestJson payload) throws BusinessApiException, TimeoutException {
        Context context = creationService.create(null, payload);

        return Response.created(asOdataId(context).toUri()).build();
    }

    @GET
    @Path("{id}")
    public EventSubscriptionDto getSubscription(@PathParam("id") Long id) {
        Context context = contextFromSubscriptionId(id);
        return getOrThrow(() -> readerService.getResource(context));
    }

    @DELETE
    @Path("{id}")
    public Response deleteSubscription(@PathParam("id") Long id) throws BusinessApiException, TimeoutException {
        removalService.perform(contextFromSubscriptionId(id));
        return noContent().build();
    }

    private Context contextFromSubscriptionId(Long id) {
        // TODO: this is a hack!
        return contextOf(id(""), EVENT_SERVICE).child(id(id), EVENT_SUBSCRIPTION);
    }
}
