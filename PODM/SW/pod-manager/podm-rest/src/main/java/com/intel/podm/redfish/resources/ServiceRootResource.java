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

import com.intel.podm.business.dto.redfish.ServiceRootContext;
import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.business.services.redfish.ServiceRootService;
import com.intel.podm.redfish.json.templates.RedfishResourceAmazingWrapper;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.common.types.redfish.ResourceNames.CHASSIS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPUTER_SYSTEM_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SERVICE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.FABRIC_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.TELEMETRY_SERVICE_RESOURCE_NAME;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@RequestScoped
@Produces(APPLICATION_JSON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ServiceRootResource extends BaseResource {
    @Inject
    private ServiceRootService readerService;

    @GET
    @Override
    public RedfishResourceAmazingWrapper get() {
        ServiceRootDto serviceRootDto = readerService.getServiceRoot();
        return new RedfishResourceAmazingWrapper(new ServiceRootContext(), serviceRootDto);
    }

    @Path("$metadata")
    public MetadataResourceProvider getRootMetadata() {
        return new MetadataResourceProvider("$metadata.xml");
    }

    @Path("metadata")
    public MetadataResource getMetadata() {
        return getResource(MetadataResource.class);
    }

    @Path(COMPOSED_NODES_RESOURCE_NAME)
    public ComposedNodeCollectionResource getComposedNodes() {
        return getResource(ComposedNodeCollectionResource.class);
    }

    @Path(COMPUTER_SYSTEM_RESOURCE_NAME)
    public ComputerSystemCollectionResource getComputerSystems() {
        return getResource(ComputerSystemCollectionResource.class);
    }

    @Path(CHASSIS_RESOURCE_NAME)
    public ChassisCollectionResource getChassis() {
        return getResource(ChassisCollectionResource.class);
    }

    @Path(MANAGERS_RESOURCE_NAME)
    public ManagerCollectionResource getManagers() {
        return getResource(ManagerCollectionResource.class);
    }

    @Path(FABRIC_RESOURCE_NAME)
    public FabricCollectionResource getFabrics() {
        return getResource(FabricCollectionResource.class);
    }

    @Path(STORAGE_SERVICES_RESOURCE_NAME)
    public ServicesCollectionResource getStorageServices() {
        return getResource(ServicesCollectionResource.class);
    }

    @Path(EVENT_SERVICE_RESOURCE_NAME)
    public EventServiceResource getEventService() {
        return getResource(EventServiceResource.class);
    }

    @Path(ETHERNET_SWITCHES_RESOURCE_NAME)
    public EthernetSwitchCollectionResource getEthernetSwitches() {
        return getResource(EthernetSwitchCollectionResource.class);
    }

    @Path(TELEMETRY_SERVICE_RESOURCE_NAME)
    public TelemetryServiceResource getTelemetryService() {
        return getResource(TelemetryServiceResource.class);
    }

    @Path("odata")
    public ODataServiceDocumentResource getOData() {
        return getResource(ODataServiceDocumentResource.class);
    }

    @Override
    protected Response createOptionsResponse() {
        return newOptionsForResourceBuilder().build();
    }
}
