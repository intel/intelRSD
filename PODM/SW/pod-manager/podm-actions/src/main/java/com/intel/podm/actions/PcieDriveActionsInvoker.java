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
import com.intel.podm.client.api.actions.PcieDriveResourceActions;
import com.intel.podm.client.api.actions.PcieDriveResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.DriveResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.mappers.redfish.DriveMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class PcieDriveActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private PcieDriveResourceActionsFactory actionsFactory;

    @Inject
    private DriveMapper driveMapper;

    @Transactional(MANDATORY)
    public void secureErase(Drive drive) throws ActionException {
        ExternalService service = drive.getService();
        requires(service != null, "There is no Service associated with selected PCIe Drive");

        try (PcieDriveResourceActions pcieDriveResourceActions = actionsFactory.create(service.getBaseUri())) {
            pcieDriveResourceActions.secureErase(drive.getSourceUri());
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "SecureErase action failed";
            logger.w(errorMessage + " on [service: {}, PCIe Drive: {}, details: {}]",
                    service.getBaseUri(),
                    drive.getSourceUri(),
                    e.getMessage());
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

    @Transactional(MANDATORY)
    public void updateDriveErased(Drive drive, boolean driveErased) throws ActionException {
        ExternalService service = drive.getService();
        requires(service != null, "There is no Service associated with selected PCIe Drive");

        try (PcieDriveResourceActions pcieDriveResourceActions = actionsFactory.create(service.getBaseUri())) {
            DriveResource driveResource = pcieDriveResourceActions.updateDriveErased(drive.getSourceUri(), driveErased);
            driveMapper.map(driveResource, drive);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update DriveErased action failed";
            logger.w(errorMessage + " on [service: {}, PCIe Drive: {}, details: {}]",
                    service.getBaseUri(),
                    drive.getSourceUri(),
                    e.getMessage());
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "DriveErased update was successful, but failed on refreshing Drive";
            logger.w(errorMessage + " on [service: {}, PCIe Drive: {}, details: {}]",
                    service.getBaseUri(),
                    drive.getSourceUri(),
                    e.getMessage());
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }
}
