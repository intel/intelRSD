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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;
import com.intel.podm.redfish.OptionsResponseBuilder;
import com.intel.podm.redfish.json.templates.actions.CreateVlanJson;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.PathParamConstants.ETHERNET_SWITCH_PORT_VLAN_ID;
import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidHttpMethod;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.net.URI.create;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.created;

@Produces(APPLICATION_JSON)
public class VlanNetworkInterfaceCollectionResource extends BaseResource {
    @Inject
    private ReaderService<RedfishVlanNetworkInterface> readerService;

    @Inject
    private CreationService<RedfishVlanNetworkInterface> creationService;

    @GET
    @Override
    public CollectionDto get() {
        return getOrThrow(() -> readerService.getCollection(getCurrentContext()));
    }

    @Path(ETHERNET_SWITCH_PORT_VLAN_ID)
    public VlanNetworkInterfaceResource getEthernetSwitchPortVlans() {
        return getResource(VlanNetworkInterfaceResource.class);
    }

    @POST
    @Consumes(APPLICATION_JSON)
    public Response createVlan(CreateVlanJson representation) throws TimeoutException, BusinessApiException {
        Context currentContext = getCurrentContext();

        if (!isPostEnabled(currentContext)) {
            throw invalidHttpMethod("Vlan cannot be created in specified resource");
        }

        Context createdContext = creationService.create(currentContext, representation);
        return created(create(asOdataId(createdContext).toString())).build();
    }

    private boolean isPostEnabled(Context currentContext) {
        return ETHERNET_SWITCH_PORT.equals(currentContext.getType());
    }

    @Override
    protected Response createOptionsResponse() {
        OptionsResponseBuilder optionsResponseBuilder = newDefaultOptionsResponseBuilder();

        if (isPostEnabled(getCurrentContext())) {
            optionsResponseBuilder.addPostMethod();
        }

        return optionsResponseBuilder
            .buildResponse();
    }
}
