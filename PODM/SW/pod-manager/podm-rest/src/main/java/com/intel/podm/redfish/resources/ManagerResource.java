/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.dto.redfish.ManagerDto;
import com.intel.podm.business.services.redfish.ManagerService;

import javax.inject.Inject;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

import static com.intel.podm.rest.resources.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.rest.resources.ResourceNames.NETWORK_PROTOCOL_RESOURCE_NAME;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class ManagerResource extends BaseResource {

    @Inject
    private ManagerService service;

    @GET
    @Override
    public ManagerDto get() {
        return getOrThrow(() -> service.getManager(getCurrentContext()));
    }

    @Path(MANAGERS_RESOURCE_NAME)
    public ManagerCollectionResource getManagers() {
        return getResource(ManagerCollectionResource.class);
    }

    @Path(NETWORK_PROTOCOL_RESOURCE_NAME)
    public NetworkProtocolResource getNetworkProtocol() {
        return getResource(NetworkProtocolResource.class);
    }

    @Path(ETHERNET_INTERFACES_RESOURCE_NAME)
    public EthernetInterfaceCollectionResource getEthernetInterfaces() {
        return getResource(EthernetInterfaceCollectionResource.class);
    }
}
