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
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.common.types.redfish.RedfishErrorResponseCarryingException;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;
import com.intel.podm.redfish.OptionsResponseBuilder;

import javax.inject.Inject;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.redfish.OptionsResponseBuilder.newDefaultOptionsResponseBuilder;
import static com.intel.podm.rest.error.PodmExceptions.invalidHttpMethod;
import static com.intel.podm.rest.error.PodmExceptions.invalidPayload;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.noContent;

@Produces(APPLICATION_JSON)
public class VlanNetworkInterfaceResource extends BaseResource {

    @Inject
    private ReaderService<RedfishVlanNetworkInterface> readerService;

    @Inject
    private RemovalService<RedfishVlanNetworkInterface> removalService;

    @GET
    @Override
    public RedfishVlanNetworkInterface get() {
        return getOrThrow(() -> readerService.getResource(getCurrentContext()));
    }

    @DELETE
    public Response deleteVlan() throws TimeoutException, BusinessApiException {
        Context currentContext = getCurrentContext();

        if (!isDeleteEnabled(currentContext)) {
            throw invalidHttpMethod("The specified vlan cannot be deleted");
        }

        try {
            removalService.perform(currentContext);
            return noContent().build();
        } catch (EntityOperationException e) {
            Throwable cause = e.getCause();
            if (cause instanceof RedfishErrorResponseCarryingException) {
                RedfishErrorResponseCarryingException ex = (RedfishErrorResponseCarryingException) cause;
                throw invalidPayload("The specified switch port vlan could not be deleted!", ex.getErrorResponse());
            } else {
                throw invalidPayload("The specified switch port vlan could not be deleted!");
            }
        }
    }

    private boolean isDeleteEnabled(Context currentContext) {
        return ETHERNET_SWITCH_PORT.equals(currentContext.getParent().getType());
    }

    @Override
    protected Response createOptionsResponse() {
        OptionsResponseBuilder optionsResponseBuilder = newDefaultOptionsResponseBuilder();

        if (isDeleteEnabled(getCurrentContext())) {
            optionsResponseBuilder.addDeleteMethod();
        }

        return optionsResponseBuilder
                .buildResponse();
    }
}
