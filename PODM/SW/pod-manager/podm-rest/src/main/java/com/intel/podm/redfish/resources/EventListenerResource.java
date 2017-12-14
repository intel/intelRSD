/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.EventDispatchingException;
import com.intel.podm.business.services.redfish.EventReceivingService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.EventsConfig;
import com.intel.podm.config.base.dto.EventsConfig.EventConfiguration;
import com.intel.podm.config.base.dto.ServiceConnectionConfig;
import com.intel.podm.redfish.json.templates.actions.EventArrayJson;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.SecurityContext;
import java.net.URI;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.valueOf;
import static com.intel.podm.config.base.dto.EventsConfig.EVENT_RECEIVING_ENDPOINT;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static java.lang.String.format;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.seeOther;
import static javax.ws.rs.core.UriBuilder.fromUri;

@RequestScoped
@Path(EVENT_RECEIVING_ENDPOINT)
public class EventListenerResource {
    @Inject
    private Logger logger;

    @Inject
    private EventReceivingService eventReceivingService;

    @Inject
    @Config
    private Holder<ServiceConnectionConfig> connectionConfig;

    @Inject
    @Config
    private Holder<EventsConfig> eventsConfig;

    @POST
    @Path("{service_type}/{uuid}")
    @Consumes(APPLICATION_JSON)
    public Response receive(EventArrayJson eventArrayJson, @Context SecurityContext securityContext,
                            @PathParam("service_type") String serviceTypeString, @PathParam("uuid") UUID uuid) throws EventDispatchingException {
        validateRequest(serviceTypeString, uuid);

        ServiceType serviceType = getServiceTypeFromString(serviceTypeString);
        if (LUI.equals(serviceType)) {
            throw notFound();
        }

        if (isDifferentChannelRequiredForServiceType(securityContext, serviceType)) {
            URI validPodmEventServiceDestination = getPodmEventServiceDestination(serviceType, uuid);
            logger.d(format("Event for %s service with UUID: %s was received via %s channel, sending redirection to: %s",
                serviceType, uuid, securityContext.isSecure() ? "secured" : "unsecured", validPodmEventServiceDestination));

            return seeOther(validPodmEventServiceDestination).build();
        }

        eventReceivingService.dispatch(uuid, eventArrayJson);
        return noContent().build();
    }

    private void validateRequest(String serviceTypeString, UUID uuid) {
        if (serviceTypeString == null || uuid == null) {
            throw notFound();
        }
    }

    private ServiceType getServiceTypeFromString(String serviceTypeString) {
        try {
            return valueOf(serviceTypeString.toUpperCase());
        } catch (IllegalArgumentException e) {
            throw notFound();
        }
    }

    private boolean isDifferentChannelRequiredForServiceType(SecurityContext securityContext, ServiceType serviceType) {
        return connectionConfig.get().getConnectionSecurity().isSslEnabledForServicesOfType(serviceType) != securityContext.isSecure();
    }

    private URI getPodmEventServiceDestination(ServiceType serviceType, UUID serviceUuid) {
        EventsConfig eventsConfiguration = eventsConfig.get();
        EventConfiguration serviceTypeEventConfig = eventsConfiguration.getEventConfigForServiceType(serviceType);
        boolean sslEnabled = connectionConfig.get().getConnectionSecurity().isSslEnabledForServicesOfType(serviceType);
        URI baseEndpoint = sslEnabled ? serviceTypeEventConfig.getSecurePodManagerEventReceivingEndpoint()
            : serviceTypeEventConfig.getDefaultPodManagerEventReceivingEndpoint();

        return fromUri(baseEndpoint).path(baseEndpoint.getPath() + '/' + serviceUuid).build();
    }
}
