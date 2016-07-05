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
import com.intel.podm.business.dto.redfish.EventServiceDto;
import com.intel.podm.business.dto.redfish.SubscriptionDto;
import com.intel.podm.common.types.Status;

import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SUBSCRIPTIONS;
import static java.util.Collections.emptyList;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class EventServiceResource extends BaseResource {

    @GET
    public EventServiceDto get() {
        return EventServiceDto.newBuilder()
                .id("EventService")
                .name("Event Service")
                .serviceEnabled(false)
                .status(Status.fromString("State=Enabled,Health=OK"))
                .eventTypesForSubscription(newArrayList("StatusChange",
                        "ResourceUpdated", "ResourceAdded", "ResourceRemoved", "Alert"))
                .build();
    }

    @GET
    @Path("/Subscriptions")
    public CollectionDto getSubscriptions() {
        return new CollectionDto(SUBSCRIPTIONS, emptyList());
    }

    @GET
    @Path("/Subscriptions/{subscriptionId}")
    public SubscriptionDto getSimpleStorage(@PathParam("subscriptionId") String subscriptionId) {
        return SubscriptionDto.newBuilder()
                .destination("http://www.dnsname.com/Destination1")
                .id(subscriptionId)
                .name("Sub name")
                .context("Context 1")
                .protocol("Redfish")
                .build();
    }

    @POST
    @Path("/Actions/EventService.SendTestEvent")
    public Object sendTestEvent(Object object) {
        return new Object();
    }
}
