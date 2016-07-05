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

package com.intel.podm.rest.resources;


import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.event.EventDataProvider;
import com.intel.podm.eventing.EventException;
import com.intel.podm.eventing.EventListenerService;
import com.intel.podm.eventing.dto.EventArrayDto;
import com.intel.podm.eventing.dto.EventDto;
import com.intel.podm.rest.representation.json.templates.EventArrayJson;

import javax.inject.Inject;
import javax.transaction.Transactional;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.SecurityContext;
import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.discovery.external.event.EventDataProvider.EVENT_ENDPOINT;
import static com.intel.podm.discovery.external.event.EventDataProvider.PSME;
import static com.intel.podm.discovery.external.event.EventDataProvider.RMM;
import static com.intel.podm.discovery.external.event.EventDataProvider.RSS;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static java.lang.String.format;
import static java.util.Objects.isNull;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.seeOther;

@Path(EVENT_ENDPOINT)
public class EventListenerResource {

    @Inject
    private Logger logger;

    @Inject
    private EventListenerService eventListenerService;

    @Inject
    private EventDataProvider eventDataProvider;

    @POST
    @Path("{service_type:(" + PSME + "|" + RSS + "|" + RMM + ")}/{uuid}")
    @Consumes(APPLICATION_JSON)
    @Transactional(REQUIRED)
    public Response receive(EventArrayJson eventArrayJson, @Context SecurityContext securityContext,
                            @PathParam("service_type") String serviceTypeString, @PathParam("uuid") UUID uuid) {

        if (eventDataProvider.isSslEnabledForService(serviceTypeString) != securityContext.isSecure()) {
            URI validPodmEventServiceDestination = eventDataProvider.getPodmEventServiceDestination(serviceTypeString, uuid);
            logger.d(format("Event for %s service with UUID: %s was received via %s channel, sending redirection to: %s",
                    serviceTypeString, uuid, securityContext.isSecure() ? "secured" : "unsecured", validPodmEventServiceDestination));

            return seeOther(validPodmEventServiceDestination).build();
        }

        EventArrayDto eventArrayDto = getEventArrayDto(eventArrayJson, uuid);

        try {
            eventListenerService.dispatch(eventArrayDto);
            return noContent().build();
        } catch (EventException e) {
            throw notFound();
        }
    }

    private EventArrayDto getEventArrayDto(EventArrayJson eventArrayJson, UUID uuid) {
        Collection<EventDto> eventDtos = getEventDtos(eventArrayJson);

        return EventArrayDto.newBuilder()
                .id(eventArrayJson.id)
                .name(eventArrayJson.name)
                .description(eventArrayJson.description)
                .serviceUuid(uuid)
                .events(eventDtos)
                .build();
    }

    private Collection<EventDto> getEventDtos(EventArrayJson eventArrayJson) {
        Collection<EventDto> eventDtos = new ArrayList<>();

        eventArrayJson.events.forEach(event -> {
            EventDto eventDto = EventDto.newBuilder()
                    .eventType(event.eventType)
                    .originOfCondition(isNull(event.originOfCondition) ? null : event.originOfCondition.toUri().toString())
                    .build();

            eventDtos.add(eventDto);
        });
        return eventDtos;
    }
}
