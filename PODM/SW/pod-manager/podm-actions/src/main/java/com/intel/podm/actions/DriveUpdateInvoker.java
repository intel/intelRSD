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

package com.intel.podm.actions;

import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.DriveResourceActions;
import com.intel.podm.client.api.actions.DriveResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.DriveResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.DriveUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class DriveUpdateInvoker {
    @Inject
    private DriveResourceActionsFactory driveResourceActionsFactory;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateDrive(Drive drive, DriveUpdateDefinition driveUpdateDefinition) throws ActionException {
        requiresNonNull(drive, "drive");
        ExternalService service = drive.getService();
        requiresNonNull(service, "service ", "there is no Service associated with selected drive");

        URI driveUri = drive.getSourceUri();

        try (DriveResourceActions actions = driveResourceActionsFactory.create(service.getBaseUri())) {
            DriveResource driveResource = actions.update(driveUri, driveUpdateDefinition);
            driveResource.getAssetTag().ifAssigned(drive::setAssetTag);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update on selected Drive failed";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, service, driveUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Update action was successful, but refreshing selected Drive failed";
            logger.i("{} on [ service: {}, path: {} ]", errorMessage, service, driveUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }
}
