/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.business.dto.redfish.DriveDto;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishDrive;
import com.intel.podm.redfish.json.templates.actions.DrivePartialRepresentation;
import com.intel.podm.redfish.json.templates.actions.constraints.DriveConstraint;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.ok;

@Produces(APPLICATION_JSON)
public class DriveResource extends BaseResource {
    @Inject
    private ReaderService<DriveDto> readerService;

    @Inject
    private UpdateService<RedfishDrive> updateService;

    @Override
    public DriveDto get() {
        return getOrThrow(() -> readerService.getResource(getCurrentContext()));
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    @Produces(APPLICATION_JSON)
    public Response updateDrive(@DriveConstraint DrivePartialRepresentation drivePartialRepresentation) throws TimeoutException, BusinessApiException {
        updateService.perform(getCurrentContext(), drivePartialRepresentation);
        return ok(get()).build();
    }

    @Path("Actions")
    public PcieDriveActionsResource getPcieDriveActionsResource() {
        return getResource(PcieDriveActionsResource.class);
    }
}
