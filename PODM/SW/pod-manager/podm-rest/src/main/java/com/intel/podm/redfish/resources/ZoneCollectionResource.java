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
import com.intel.podm.business.dto.ZoneDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.ZoneCreationInterface;
import com.intel.podm.redfish.json.templates.actions.CreateZoneJson;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.PathParamConstants.ZONE_ID;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@RequestScoped
@Produces(APPLICATION_JSON)
public class ZoneCollectionResource extends BaseResource {
    @Inject
    private ReaderService<ZoneDto> readerService;

    @Inject
    private CreationService<ZoneCreationInterface> creationService;

    @Override
    public CollectionDto get() {
        return getOrThrow(() -> readerService.getCollection(getCurrentContext()));
    }

    @Path(ZONE_ID)
    public ZoneResource getZone() {
        return getResource(ZoneResource.class);
    }

    @POST
    @Consumes(APPLICATION_JSON)
    public Response createZone(CreateZoneJson representation) throws BusinessApiException, TimeoutException {
        Context context = getCurrentContext();

        Context createdZoneContext = creationService.create(context, representation);
        return Response.created(createdZoneContext.asOdataId().toUri()).build();
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder().addPostMethod().build();
    }
}
