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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishErrorResponseCarryingException;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;
import com.intel.podm.redfish.json.templates.actions.CreateEthernetSwitchPortActionJson;
import com.intel.podm.rest.error.PodmExceptions;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_PORT_ID;
import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.net.URI.create;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.created;

@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortCollectionResource extends BaseResource {
    @Inject
    private ReaderService<EthernetSwitchPortDto> readerService;

    @Inject
    private CreationService<RedfishEthernetSwitchPort> ethernetSwitchPortCreationService;

    @GET
    @Override
    public CollectionDto get() {
        return getOrThrow(() -> readerService.getCollection(getCurrentContext()));
    }

    @POST
    @Consumes(APPLICATION_JSON)
    public Response createSwitchPort(CreateEthernetSwitchPortActionJson representation) throws TimeoutException, BusinessApiException {
        try {
            Context createdPortContext = ethernetSwitchPortCreationService.create(getCurrentContext(), representation);
            Object oDataId = ofNullable(asOdataId(createdPortContext)).orElseThrow(PodmExceptions::internalServerError);
            return created(create(oDataId.toString())).build();
        } catch (EntityOperationException e) {
            Throwable cause = e.getCause();
            if (cause instanceof RedfishErrorResponseCarryingException) {
                RedfishErrorResponseCarryingException ex = (RedfishErrorResponseCarryingException) cause;
                throw invalidPayload(e.getMessage(), ex.getErrorResponse());
            } else {
                throw invalidPayload(e.getMessage());
            }
        }
    }

    @Override
    protected Response createOptionsResponse() {
        return newDefaultOptionsResponseBuilder()
            .addPostMethod()
            .buildResponse();
    }

    @Path(ETHERNET_SWITCH_PORT_ID)
    public EthernetSwitchPortResource getEthernetSwitchPort() {
        return getResource(EthernetSwitchPortResource.class);
    }
}
