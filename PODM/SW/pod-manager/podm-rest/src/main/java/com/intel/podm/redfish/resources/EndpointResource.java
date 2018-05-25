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
import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishEndpoint;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.UpdateEndpointJson;
import com.intel.podm.redfish.json.templates.actions.constraints.EndpointUpdateConstraint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
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
public class EndpointResource extends BaseResource {
    @Inject
    private ReaderService<EndpointDto> readerService;

    @Inject
    private UpdateService<RedfishEndpoint> endpointUpdateService;

    @Inject
    private RemovalService<EndpointDto> removalService;

    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        EndpointDto endpointDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, endpointDto);
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateEndpoint(@EndpointUpdateConstraint UpdateEndpointJson updateEndpointJson) throws BusinessApiException, TimeoutException {
        endpointUpdateService.perform(getCurrentContext(), updateEndpointJson);

        return ok(get()).build();
    }

    @DELETE
    @Override
    public Response delete() throws TimeoutException, BusinessApiException {
        removalService.perform(getCurrentContext());
        return noContent().build();
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addPatchMethod()
            .addDeleteMethod()
            .build();
    }
}
