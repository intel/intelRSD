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

package com.intel.podm.client.actions;

import com.intel.podm.client.actions.requests.PcieDriveErasedUpdateRequest;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.PcieDriveResourceActions;
import com.intel.podm.client.api.resources.redfish.DriveResource;

import java.net.URI;

import static java.net.URI.create;

public class PcieDriveResourceActionsImpl implements PcieDriveResourceActions {
    private static final String PCIE_DRIVE_SECURE_ERASE_PATH_PART = "/Actions/Drive.SecureErase";
    private final WebClient webClient;

    public PcieDriveResourceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public void secureErase(URI pcieDriveUri) throws ExternalServiceApiActionException {
        URI secureEraseUri = create(pcieDriveUri + PCIE_DRIVE_SECURE_ERASE_PATH_PART);
        webClient.post(secureEraseUri, null);
    }

    @Override
    public DriveResource updateDriveErased(URI pcieDriveUri, boolean driveErased)
        throws ExternalServiceApiActionException, ExternalServiceApiReaderException {

        return (DriveResource) webClient.patchAndRetrieve(pcieDriveUri, new PcieDriveErasedUpdateRequest(driveErased));
    }

    @Override
    public DriveResource getPcieDrive(URI pcieDriveUri) throws ExternalServiceApiReaderException {
        if (pcieDriveUri == null) {
            throw new ExternalServiceApiReaderException("Could not read PCIe Drive", null);
        }

        return (DriveResource) webClient.get(pcieDriveUri);
    }

    @Override
    public void close() {
        webClient.close();
    }
}
