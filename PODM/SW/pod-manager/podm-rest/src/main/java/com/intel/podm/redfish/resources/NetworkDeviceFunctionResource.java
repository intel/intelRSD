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
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.redfish.NetworkDeviceFunctionDto;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;
import com.intel.podm.common.types.redfish.RedfishErrorResponseCarryingException;
import com.intel.podm.redfish.json.templates.actions.NetworkDeviceFunctionPartialRepresentation;
import com.intel.podm.redfish.json.templates.actions.constraints.NetworkDeviceFunctionConstraint;

import javax.inject.Inject;
import javax.ws.rs.Consumes;
import javax.ws.rs.PATCH;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.ok;

@Produces(APPLICATION_JSON)
public class NetworkDeviceFunctionResource extends BaseResource {
    @Inject
    private ReaderService<NetworkDeviceFunctionDto> readerService;

    @Inject
    private UpdateService<RedfishNetworkDeviceFunction> updateService;

    @Override
    public NetworkDeviceFunctionDto get() {
        return getOrThrow(() -> readerService.getResource(getCurrentContext()));
    }

    @PATCH
    @Consumes(APPLICATION_JSON)
    public Response updateNetworkDeviceFunction(@NetworkDeviceFunctionConstraint NetworkDeviceFunctionPartialRepresentation updateNetworkDeviceActionJson)
        throws TimeoutException, BusinessApiException {
        try {
            updateService.perform(getCurrentContext(), updateNetworkDeviceActionJson);
            return ok(get()).build();
        } catch (EntityOperationException e) {
            Throwable cause = e.getCause();
            if (cause instanceof RedfishErrorResponseCarryingException) {
                RedfishErrorResponseCarryingException ex = (RedfishErrorResponseCarryingException) cause;
                throw invalidPayload("The specified network device function could not be updated!", ex.getErrorResponse());
            } else {
                throw invalidPayload("The specified network device function could not be updated!");
            }
        }
    }

    @Override
    protected Response createOptionsResponse() {
        return newDefaultOptionsResponseBuilder()
            .addPatchMethod()
            .buildResponse();
    }
}
