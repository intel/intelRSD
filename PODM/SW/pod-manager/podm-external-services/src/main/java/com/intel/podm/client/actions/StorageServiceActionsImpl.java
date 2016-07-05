/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.client.WebClientImpl;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.LogicalDriveResourceCreationRequest;
import com.intel.podm.client.api.actions.RemoteTargetResourceCreationRequest;
import com.intel.podm.client.api.actions.StorageServiceActions;
import com.intel.podm.client.api.resources.redfish.LogicalDriveResource;
import com.intel.podm.common.types.State;

import java.net.URI;
import java.util.Optional;

import static java.net.URI.create;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;

public class StorageServiceActionsImpl implements StorageServiceActions {
    private static final int CHECK_DELAY_MS = 5000;

    private WebClient webClient;

    public StorageServiceActionsImpl(URI baseUri) {
        this(WebClientImpl.createRetryable(baseUri));
    }

    public StorageServiceActionsImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public URI createBootableLogicalDrive(URI storageServiceUri, LogicalDriveResourceCreationRequest request)
            throws ExternalServiceApiActionException, InterruptedException {
        URI logicalDriveUri = createLogicalDrive(storageServiceUri, request);
        URI path = create(logicalDriveUri.getPath());

        try (WebClient client = WebClientImpl.createRetryable(webClient.getBaseUri())) {
            LogicalDriveResource drive = (LogicalDriveResource) client.get(path);
            if (!drive.isBootable()) {
                throw new ExternalServiceApiActionException("Logical Drive was created but is not bootable.", logicalDriveUri);
            }
            return logicalDriveUri;
        } catch (ExternalServiceApiReaderException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public URI createLogicalDrive(URI storageServiceUri, LogicalDriveResourceCreationRequest request)
            throws ExternalServiceApiActionException, InterruptedException {
        URI targetUri = create(storageServiceUri.toString() + "/LogicalDrives");
        return waitForEnabledState(webClient.post(targetUri, request));
    }

    private URI waitForEnabledState(URI resourceUri) throws ExternalServiceApiActionException, InterruptedException {
        URI path = create(resourceUri.getPath());

        while (true) {
            Optional<State> driveState = readDriveState(webClient.getBaseUri(), path);

            if (driveState.isPresent()) {
                switch (driveState.get()) {
                    case ENABLED:
                        return resourceUri;
                    case ABSENT:
                        throw new ExternalServiceApiActionException("Error occurred during creation of logical drive", resourceUri);
                    default:
                        break;
                }
            }

            Thread.sleep(CHECK_DELAY_MS);
        }
    }

    private Optional<State> readDriveState(URI baseUri, URI path) {
        try (WebClient webClient = WebClientImpl.createRetryable(baseUri)) {
            LogicalDriveResource drive = (LogicalDriveResource) webClient.get(path);
            return ofNullable(drive.getStatus().getState());
        } catch (ExternalServiceApiReaderException e) {
            return empty();
        }
    }

    @Override
    public URI createRemoteTarget(URI storageServiceUri, RemoteTargetResourceCreationRequest request) throws ExternalServiceApiActionException {
        URI targetUri = create(storageServiceUri.toString() + "/Targets");
        return webClient.post(targetUri, request);
    }
}
