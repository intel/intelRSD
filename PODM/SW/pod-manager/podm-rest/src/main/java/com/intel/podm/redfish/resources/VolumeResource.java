/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.dto.actions.InitializeActionDto;
import com.intel.podm.business.dto.redfish.VolumeDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.requests.InitializeRequest;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishVolume;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.VolumePartialRepresentation;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.redfish.ActionsResourceNames.ACTIONS_VOLUME_INITIALIZE;
import static com.intel.podm.common.types.redfish.ResourceNames.VOLUME_METRICS_RESOURCE_NAME;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VolumeResource extends BaseResource {

    @Inject
    private ReaderService<VolumeDto> readerService;

    @Inject
    private RemovalService<VolumeDto> removalService;

    @Inject
    private ActionService<InitializeRequest> actionService;

    @Inject
    private UpdateService<RedfishVolume> updateService;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        VolumeDto volumeDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, volumeDto);
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    @Produces(APPLICATION_JSON)
    public Response updateVolume(VolumePartialRepresentation volumePartialRepresentation) throws BusinessApiException, TimeoutException {
        updateService.perform(getCurrentContext(), volumePartialRepresentation);
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

    @POST
    @Consumes(APPLICATION_JSON)
    @Path(ACTIONS_VOLUME_INITIALIZE)
    public Response initialize(InitializeActionDto initializeActionDto) throws TimeoutException, BusinessApiException {
        actionService.perform(getCurrentContext(), initializeActionDto);
        return noContent().build();
    }

    @Path(VOLUME_METRICS_RESOURCE_NAME)
    public VolumeMetricsResource getVolumeMetrics() {
        return getResource(VolumeMetricsResource.class);
    }
}
