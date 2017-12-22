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
import com.intel.podm.business.dto.EthernetSwitchPortDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;
import com.intel.podm.redfish.json.templates.actions.CreateEthernetSwitchPortActionJson;
import com.intel.podm.rest.error.PodmExceptions;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.net.URI;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_PORT_ID;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.created;

@RequestScoped
@Produces(APPLICATION_JSON)
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
        Context createdPortContext = ethernetSwitchPortCreationService.create(getCurrentContext(), representation);
        Object oDataId = ofNullable(asOdataId(createdPortContext)).orElseThrow(PodmExceptions::internalServerError);
        return created(URI.create(oDataId.toString())).build();
    }

    @Path(ETHERNET_SWITCH_PORT_ID)
    public EthernetSwitchPortResource getEthernetSwitchPort() {
        return getResource(EthernetSwitchPortResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addPostMethod()
            .build();
    }
}
