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
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.dto.EthernetSwitchPortDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;
import com.intel.podm.redfish.OptionsResponseBuilder;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.UpdateEthernetSwitchPortActionJson;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.PortClass.LOGICAL;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_METRICS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidHttpMethod;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortResource extends BaseResource {
    @Inject
    private ReaderService<EthernetSwitchPortDto> readerService;

    @Inject
    private UpdateService<RedfishEthernetSwitchPort> updateService;

    @Inject
    private RemovalService<RedfishEthernetSwitchPort> removalService;

    @Inject
    private Logger logger;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        EthernetSwitchPortDto ethernetSwitchPortDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, ethernetSwitchPortDto);
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateSwitchPort(UpdateEthernetSwitchPortActionJson updateSwitchPortActionJson) throws TimeoutException, BusinessApiException {
        updateService.perform(getCurrentContext(), updateSwitchPortActionJson);
        return ok(get()).build();
    }

    @DELETE
    public Response deleteSwitchPort() throws TimeoutException, BusinessApiException {
        try {
            removalService.perform(getCurrentContext());
            return noContent().build();
        } catch (RequestValidationException e) {
            throw invalidHttpMethod("The specified switch port could not be deleted!", e);
        }
    }

    @Path(ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME)
    public VlanNetworkInterfaceCollectionResource getEthernetSwitchPortVlans() {
        return getResource(VlanNetworkInterfaceCollectionResource.class);
    }

    @Path(ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME)
    public EthernetSwitchStaticMacCollectionResource getEthernetSwitchStaticMacs() {
        return getResource(EthernetSwitchStaticMacCollectionResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        OptionsResponseBuilder optionsResponseBuilder = newOptionsForResourceBuilder().addPatchMethod();

        try {
            EthernetSwitchPortDto port = readerService.getResource(getCurrentContext());
            if (LOGICAL.equals(port.getPortClass())) {
                optionsResponseBuilder.addDeleteMethod();
            }
        } catch (ContextResolvingException e) {
            logger.w("EthernetSwitchPort was not found in the specified context: {}", e.getContext());
            optionsResponseBuilder.addDeleteMethod();
        }

        return optionsResponseBuilder.build();
    }

    @Path(ETHERNET_SWITCH_PORT_METRICS_RESOURCE_NAME)
    public EthernetSwitchPortMetricsResource getEthernetSwitchPortMetrics() {
        return getResource(EthernetSwitchPortMetricsResource.class);
    }
}
