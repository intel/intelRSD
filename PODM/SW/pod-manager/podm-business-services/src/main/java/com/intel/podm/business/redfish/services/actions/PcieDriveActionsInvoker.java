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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.PcieDriveErasedUpdateRequest;
import com.intel.podm.client.resources.redfish.DriveResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.mappers.redfish.DriveMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class PcieDriveActionsInvoker {
    private static final String PCIE_DRIVE_SECURE_ERASE_PATH_PART = "/Actions/Drive.SecureErase";

    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private DriveMapper driveMapper;

    @Transactional(MANDATORY)
    public void secureErase(Drive drive) throws EntityOperationException {
        ExternalService service = drive.getService();
        requires(service != null, "There is no Service associated with selected PCIe Drive");

        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            URI secureEraseUri = URI.create(drive.getSourceUri() + PCIE_DRIVE_SECURE_ERASE_PATH_PART);
            webClient.post(secureEraseUri, null);
        } catch (WebClientRequestException e) {
            String errorMessage = "SecureErase action failed";
            logger.w(errorMessage + " on [service: {}, PCIe Drive: {}, details: {}]",
                service.getBaseUri(),
                drive.getSourceUri(),
                e.getMessage());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void updateDriveErased(Drive drive, boolean driveErased) throws EntityOperationException {
        ExternalService service = drive.getService();
        requires(service != null, "There is no Service associated with selected PCIe Drive");

        DriveResource driveResource = performUpdateDriveErasedAction(drive, driveErased, service);
        driveMapper.map(driveResource, drive);
    }

    private DriveResource performUpdateDriveErasedAction(Drive drive, boolean driveErased, ExternalService service) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return (DriveResource) webClient.patchAndRetrieve(drive.getSourceUri(), new PcieDriveErasedUpdateRequest(driveErased));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update DriveErased action failed";
            logger.w(errorMessage + " on [service: {}, PCIe Drive: {}, details: {}]",
                service.getBaseUri(),
                drive.getSourceUri(),
                e.getMessage());
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
