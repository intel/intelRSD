/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.actions.ActionException;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.redfish.services.EthernetSwitchPortActionsService;
import com.intel.podm.business.services.redfish.EthernetSwitchPortService;
import com.intel.podm.redfish.OptionsResponseBuilder;
import com.intel.podm.redfish.json.templates.actions.UpdateEthernetSwitchPortActionJson;
import com.intel.podm.rest.resources.ResourceNames;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.common.types.PortClass.LOGICAL;
import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidHttpMethod;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static com.intel.podm.rest.error.PodmExceptions.notFound;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
public class EthernetSwitchPortResource extends BaseResource {

    @Inject
    private EthernetSwitchPortService service;

    @Inject
    private EthernetSwitchPortActionsService actionService;

    @GET
    @Override
    public EthernetSwitchPortDto get() {
        return getOrThrow(() -> service.getEthernetSwitchPort(getCurrentContext()));
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateSwitchPort(UpdateEthernetSwitchPortActionJson updateSwitchPortActionJson) {
        try {
            actionService.updateSwitchPort(getCurrentContext(), updateSwitchPortActionJson);
            return noContent().build();
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (ActionException e) {
            throw invalidPayload("Switch Port modification action could not be completed!", e.getErrorResponse());
        }
    }

    @DELETE
    @Override
    public Response delete() {
        EthernetSwitchPortDto port = this.get();

        if (!LOGICAL.equals(port.getPortClass())) {
            throw invalidHttpMethod("The specified switch port could not be deleted!");
        }

        try {
            actionService.deleteSwitchPort(getCurrentContext());
        } catch (EntityNotFoundException e) {
            throw notFound();
        } catch (ActionException e) {
            throw invalidPayload("The specified switch port could not be deleted!", e.getErrorResponse());
        }

        return noContent().build();
    }

    @Path(ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME)
    public EthernetSwitchPortVlanCollectionResource getEthernetSwitchPortVlans() {
        return getResource(EthernetSwitchPortVlanCollectionResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        EthernetSwitchPortDto port = this.get();

        OptionsResponseBuilder optionsResponseBuilder = newDefaultOptionsResponseBuilder()
                .addPatchMethod();

        if (LOGICAL.equals(port.getPortClass())) {
            optionsResponseBuilder.addDeleteMethod();
        }

        return optionsResponseBuilder.buildResponse();
    }
}
