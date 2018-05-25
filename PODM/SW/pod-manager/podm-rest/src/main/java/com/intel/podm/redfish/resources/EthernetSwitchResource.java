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
import com.intel.podm.business.dto.EthernetSwitchDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitch;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.EthernetSwitchPartialRepresentation;
import com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchConstraint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.redfish.ResourceNames.ACLS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_METRICS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PORTS_RESOURCE_NAME;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchResource extends BaseResource {

    @Inject
    private ReaderService<EthernetSwitchDto> readerService;

    @Inject
    private UpdateService<RedfishEthernetSwitch> updateService;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        EthernetSwitchDto ethernetSwitchDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, ethernetSwitchDto);
    }

    @Path(ACLS_RESOURCE_NAME)
    public EthernetSwitchAclCollectionResource getEthernetSwitchAcls() {
        return getResource(EthernetSwitchAclCollectionResource.class);
    }

    @Path(PORTS_RESOURCE_NAME)
    public EthernetSwitchPortCollectionResource getEthernetSwitchPorts() {
        return getResource(EthernetSwitchPortCollectionResource.class);
    }

    @Path(ETHERNET_SWITCH_METRICS_RESOURCE_NAME)
    public EthernetSwitchMetricsResource getEthernetSwitchMetrics() {
        return getResource(EthernetSwitchMetricsResource.class);
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateEthernetSwitch(@EthernetSwitchConstraint EthernetSwitchPartialRepresentation representation)
        throws TimeoutException, BusinessApiException {

        updateService.perform(getCurrentContext(), representation);
        return ok(get()).build();
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
                .addPatchMethod()
                .build();
    }
}
