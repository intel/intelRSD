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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.DriveUpdateRequest;
import com.intel.podm.client.resources.redfish.DriveResource;
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
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateDrive(Drive drive, DriveUpdateDefinition driveUpdateDefinition) throws EntityOperationException {
        requiresNonNull(drive, "drive");
        ExternalService service = drive.getService();
        requiresNonNull(service, "service ", "there is no Service associated with selected drive");

        DriveResource driveResource = performUpdateDriveAction(driveUpdateDefinition, service, drive.getSourceUri());
        driveResource.getAssetTag().ifAssigned(drive::setAssetTag);
    }

    private DriveResource performUpdateDriveAction(DriveUpdateDefinition driveUpdateDefinition, ExternalService service, URI driveUri)
        throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return webClient.patchAndRetrieve(driveUri, new DriveUpdateRequest(driveUpdateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Drive failed";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, service, driveUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
