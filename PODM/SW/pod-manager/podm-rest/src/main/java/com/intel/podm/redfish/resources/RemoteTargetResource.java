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
import com.intel.podm.business.dto.redfish.RemoteTargetDto;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishRemoteTarget;
import com.intel.podm.redfish.json.templates.actions.RemoteTargetPartialRepresentation;
import com.intel.podm.redfish.json.templates.actions.constraints.RemoteTargetConstraint;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.PATCH;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import java.util.concurrent.TimeoutException;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.ok;

@Produces(APPLICATION_JSON)
public class RemoteTargetResource extends BaseResource {

    @Inject
    private ReaderService<RemoteTargetDto> readerService;

    @Inject
    private UpdateService<RedfishRemoteTarget> updateService;

    @Override
    public RemoteTargetDto get() {
        return getOrThrow(() -> readerService.getResource(getCurrentContext()));
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateRemoteTarget(@RemoteTargetConstraint RemoteTargetPartialRepresentation partialRepresentation)
            throws TimeoutException, BusinessApiException {
        updateService.perform(getCurrentContext(), partialRepresentation);
        return ok(get()).build();
    }
}
