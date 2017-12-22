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
import com.intel.podm.business.dto.EthernetSwitchStaticMacDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchStaticMac;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.EthernetSwitchStaticMacJson;
import com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchStaticMacConstraint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.PATCH;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
public class EthernetSwitchStaticMacResource extends BaseResource {
    @Inject
    private ReaderService<EthernetSwitchStaticMacDto> readerService;

    @Inject
    private RemovalService<EthernetSwitchStaticMacDto> removalService;

    @Inject
    private UpdateService<RedfishEthernetSwitchStaticMac> updateService;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        EthernetSwitchStaticMacDto ethernetSwitchStaticMacDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, ethernetSwitchStaticMacDto);
    }

    @DELETE
    @Override
    public Response delete() throws TimeoutException, BusinessApiException {
        removalService.perform(getCurrentContext());
        return noContent().build();
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response update(@EthernetSwitchStaticMacConstraint EthernetSwitchStaticMacJson json) throws BusinessApiException, TimeoutException {
        updateService.perform(getCurrentContext(), json);
        return ok(get()).build();
    }


    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addDeleteMethod()
            .addPatchMethod()
            .build();
    }
}
