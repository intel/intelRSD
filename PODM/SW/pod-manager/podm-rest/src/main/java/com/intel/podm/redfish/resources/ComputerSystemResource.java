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
import com.intel.podm.business.dto.ComputerSystemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;
import com.intel.podm.redfish.json.templates.actions.ComputerSystemPartialRepresentation;
import com.intel.podm.redfish.json.templates.actions.constraints.ComputerSystemConstraint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.inject.Named;
import javax.ws.rs.PATCH;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.redfish.ResourceNames.COMPUTER_SYSTEM_METRICS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MEMORY_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PROCESSORS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.SIMPLE_STORAGE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_RESOURCE_NAME;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.ok;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemResource extends BaseResource {
    @Inject
    private ReaderService<ComputerSystemDto> readerService;

    @Inject
    @Named("ComputerSystem")
    private UpdateService<RedfishComputerSystem> computerSystemUpdateService;

    @Override
    public RedfishResourceAmazingWrapper get() {
        Context context = getCurrentContext();
        ComputerSystemDto computerSystemDto = getOrThrow(() -> readerService.getResource(context));
        return new RedfishResourceAmazingWrapper(context, computerSystemDto);
    }

    @PATCH
    @Produces(APPLICATION_JSON)
    public Response patchComputerSystem(@ComputerSystemConstraint ComputerSystemPartialRepresentation representation)
        throws TimeoutException, BusinessApiException {

        computerSystemUpdateService.perform(getCurrentContext(), representation);
        return ok(get()).build();
    }

    @Path(PROCESSORS_RESOURCE_NAME)
    public ProcessorsCollectionResource getProcessorsCollection() {
        return getResource(ProcessorsCollectionResource.class);
    }

    @Path(SIMPLE_STORAGE_RESOURCE_NAME)
    public SimpleStorageCollectionResource getSimpleStorage() {
        return getResource(SimpleStorageCollectionResource.class);
    }

    @Path(STORAGE_RESOURCE_NAME)
    public StorageCollectionResource getStorage() {
        return getResource(StorageCollectionResource.class);
    }

    @Path(MEMORY_RESOURCE_NAME)
    public MemoryCollectionResource getMemoryCollection() {
        return getResource(MemoryCollectionResource.class);
    }

    @Path(ETHERNET_INTERFACES_RESOURCE_NAME)
    public EthernetInterfaceCollectionResource getEthernetInterfacesCollection() {
        return getResource(EthernetInterfaceCollectionResource.class);
    }

    @Path(NETWORK_INTERFACES_RESOURCE_NAME)
    public NetworkInterfaceCollectionResource getNetworkInterfacesCollection() {
        return getResource(NetworkInterfaceCollectionResource.class);
    }

    @Path(COMPUTER_SYSTEM_METRICS_RESOURCE_NAME)
    public ComputerSystemMetricsResource getComputerSystemMetrics() {
        return getResource(ComputerSystemMetricsResource.class);
    }

    @Path("Actions")
    public ComputerSystemActionsResource getComputerSystemActionsResource() {
        return getResource(ComputerSystemActionsResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder()
            .addPatchMethod()
            .build();
    }
}
