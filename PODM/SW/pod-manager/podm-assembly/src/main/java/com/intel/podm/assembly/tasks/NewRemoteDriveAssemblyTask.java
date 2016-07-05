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

package com.intel.podm.assembly.tasks;

import com.intel.podm.assembly.AssemblyException;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.actions.LogicalDriveResourceCreationRequest;
import com.intel.podm.client.api.actions.RemoteTargetResourceCreationRequest;
import com.intel.podm.client.api.actions.StorageServiceActions;
import com.intel.podm.client.api.actions.StorageServiceActionsFactory;
import com.intel.podm.client.api.actions.StorageServiceSubresourceCreationRequestFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.DiscoveryScheduler;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.UUID;

import static com.intel.podm.common.types.ReplicationMethod.SNAPSHOT;
import static java.net.URI.create;

@Dependent
public class NewRemoteDriveAssemblyTask extends NodeAssemblyTask {
    public static final int CHECK_INTERVAL = 5000;

    @Inject
    private StorageServiceActionsFactory actionsFactory;

    @Inject
    private StorageServiceSubresourceCreationRequestFactory creationRequestFactory;

    @Inject
    private Logger logger;

    @Inject
    private TargetAttacher targetAttacher;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    private URI masterDriveSourceUri;
    private RequestedRemoteDrive requestedRemoteDrive;
    private URI rssServiceUri;
    private URI storageServiceSourceUri;
    private URI lvgSourceUri;
    private UUID rssServiceUuid;

    public void setMasterDriveSourceUri(URI masterDriveSourceUri) {
        this.masterDriveSourceUri = masterDriveSourceUri;
    }

    public void setRequestedRemoteDrive(RequestedRemoteDrive requestedRemoteDrive) {
        this.requestedRemoteDrive = requestedRemoteDrive;
    }

    public void setRssServiceUri(URI rssServiceUri) {
        this.rssServiceUri = rssServiceUri;
    }

    public void setRssServiceUuid(UUID rssServiceUuid) {
        this.rssServiceUuid = rssServiceUuid;
    }

    public void setStorageServiceSourceUri(URI storageServiceSourceUri) {
        this.storageServiceSourceUri = storageServiceSourceUri;
    }

    public void setLvgSourceUri(URI lvgSourceUri) {
        this.lvgSourceUri = lvgSourceUri;
    }

    @Override
    public void run() {
        logger.d("Running for Node: {}", nodeId);

        try {
            URI newLogicalDriveUri = sendLogicalDriveCreationRequest();
            URI newRemoteTargetUri = sendRemoteTargetCreationRequest(newLogicalDriveUri);
            URI remoteTargetRelativeUri = create(newRemoteTargetUri.getPath());

            discoveryScheduler.triggerDiscovery(rssServiceUuid);

            waitForRemoteTarget(remoteTargetRelativeUri);
            logger.d("Finished for Node: {}", nodeId);
        } catch (AssemblyException e) {
            logger.e("Remote target creation failed for Node: {}, error: {}, details: {}", nodeId, e.getErrorResponse(), e.getMessage());
            throw new RuntimeException(e);
        } catch (InterruptedException e) {
            logger.e("Remote target creation failed for Node: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void waitForRemoteTarget(URI newRemoteTargetUri) throws AssemblyException, InterruptedException {
        logger.d("Waiting for remote target: {}", newRemoteTargetUri);
        while (!targetAttacher.attachIfCompleted(nodeId, rssServiceUuid, newRemoteTargetUri)) {
            Thread.sleep(CHECK_INTERVAL);
        }
    }

    private URI sendLogicalDriveCreationRequest() throws AssemblyException, InterruptedException {
        LogicalDriveResourceCreationRequest request = buildLogicalDriveRequest();

        try {
            StorageServiceActions rssActions = actionsFactory.create(rssServiceUri);
            return rssActions.createBootableLogicalDrive(storageServiceSourceUri, request);
        } catch (ExternalServiceApiActionException e) {
            throw new AssemblyException("Logical Drive Creation Failed!", e.getErrorResponse(), e);
        }
    }

    private URI sendRemoteTargetCreationRequest(URI newLogicalDriveUri) throws AssemblyException {
        URI uri = create(newLogicalDriveUri.getPath());
        RemoteTargetResourceCreationRequest request = buildRemoteTargetRequest(uri);

        try {
            StorageServiceActions rssActions = actionsFactory.create(rssServiceUri);
            return rssActions.createRemoteTarget(storageServiceSourceUri, request);
        } catch (ExternalServiceApiActionException e) {
            throw new AssemblyException("Remote Target Creation Failed!", e.getErrorResponse(), e);
        }
    }

    private LogicalDriveResourceCreationRequest buildLogicalDriveRequest() {
        LogicalDriveResourceCreationRequest request = creationRequestFactory.getLogicalDriveCreationRequest();
        request.setSnapshot(requestedRemoteDrive.getMaster().getType() == SNAPSHOT);
        request.setCapacityGib(requestedRemoteDrive.getCapacityGib());
        request.setMaster(masterDriveSourceUri);
        request.setLvg(lvgSourceUri);

        return request;
    }

    private RemoteTargetResourceCreationRequest buildRemoteTargetRequest(URI newLogicalDriveUri) {
        RemoteTargetResourceCreationRequest request = creationRequestFactory.getRemoteTargetCreationRequest();
        request.setInitiatorIqn("ALL");
        request.setLunInfo(newLogicalDriveUri, requestedRemoteDrive.getIscsiAddress());

        return request;
    }
}
