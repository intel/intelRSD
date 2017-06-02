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

import com.intel.podm.business.dto.redfish.ServiceDto;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

import static com.intel.podm.common.types.redfish.ResourceNames.LOGICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PHYSICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.REMOTE_TARGETS_RESOURCE_NAME;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class ServiceResource extends BaseResource {
    @Inject
    private ReaderService<ServiceDto> readerService;

    @Override
    public ServiceDto get() {
        return getOrThrow(() -> readerService.getResource(getCurrentContext()));
    }

    @Path(REMOTE_TARGETS_RESOURCE_NAME)
    public RemoteTargetsCollectionResource getRemoteTargets() {
        return getResource(RemoteTargetsCollectionResource.class);
    }

    @Path(LOGICAL_DRIVES_RESOURCE_NAME)
    public LogicalDrivesCollectionResource getLogicalDrives() {
        return getResource(LogicalDrivesCollectionResource.class);
    }

    @Path(PHYSICAL_DRIVES_RESOURCE_NAME)
    public PhysicalDrivesCollectionResource getPhysicalDrives() {
        return getResource(PhysicalDrivesCollectionResource.class);
    }
}
