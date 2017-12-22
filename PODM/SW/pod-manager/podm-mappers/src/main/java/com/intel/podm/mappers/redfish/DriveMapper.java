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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.client.resources.redfish.DriveResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.IdentifierMapper;
import com.intel.podm.mappers.subresources.RedfishLocationMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class DriveMapper extends EntityMapper<DriveResource, Drive> {
    @Inject
    IdentifierMapper identifierMapper;

    @Inject
    RedfishLocationMapper redfishLocationMapper;

    public DriveMapper() {
        super(DriveResource.class, Drive.class);
    }

    @Override
    protected void performNotAutomatedMapping(DriveResource sourceDrive, Drive targetDrive) {
        super.performNotAutomatedMapping(source, target);
        sourceDrive.getIdentifiers().ifAssigned(identifiers ->
            identifierMapper.map(identifiers, targetDrive.getIdentifiers(), targetDrive::addIdentifier)
        );
        sourceDrive.getLocation().ifAssigned(locations ->
            redfishLocationMapper.map(locations, targetDrive.getLocation(), targetDrive::addLocation)
        );

        if (targetDrive.getEraseOnDetach() == null) {
            targetDrive.setEraseOnDetach(true);
        }
    }
}
