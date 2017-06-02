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

package com.intel.podm.assembly.tasks;

import com.intel.podm.assembly.AssemblyException;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.actions.LogicalDriveResourceCreationRequest;
import com.intel.podm.client.api.actions.RemoteTargetResourceCreationRequest;
import com.intel.podm.client.api.actions.StorageServiceActions;
import com.intel.podm.client.api.actions.StorageServiceActionsFactory;
import com.intel.podm.client.api.actions.StorageServiceSubresourceCreationRequestFactory;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.discovery.external.DiscoveryRunner;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.UUID;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.ReplicationMethod.SNAPSHOT;
import static java.lang.String.format;
import static java.net.URI.create;
import static java.time.Duration.ofMinutes;
import static java.util.UUID.randomUUID;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class NewRemoteDriveAssemblyTask extends NodeAssemblyTask {
    private static final int MINUTES_TO_WAIT_FOR_DISCOVERY = 5;

    @Inject
    private StorageServiceActionsFactory actionsFactory;

    @Inject
    private StorageServiceSubresourceCreationRequestFactory creationRequestFactory;

    @Inject
    private Logger logger;

    @Inject
    private TargetAttacher targetAttacher;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private TaskCoordinator taskCoordinator;

    private URI masterDriveSourceUri;
    private RequestedNode.RemoteDrive requestedRemoteDrive;
    private URI rssServiceUri;
    private URI storageServiceSourceUri;
    private URI lvgSourceUri;
    private UUID rssServiceUuid;

    public void setMasterDriveSourceUri(URI masterDriveSourceUri) {
        this.masterDriveSourceUri = masterDriveSourceUri;
    }

    public void setRequestedRemoteDrive(RequestedNode.RemoteDrive requestedRemoteDrive) {
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
    @TimeMeasured(tag = "[AssemblyTask]")
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    public void run() {
        logger.d("Running for Node: {}", nodeId);

        try {
            URI newLogicalDriveUri = sendLogicalDriveCreationRequest();
            URI newRemoteTargetUri = sendRemoteTargetCreationRequest(newLogicalDriveUri);
            URI remoteTargetRelativeUri = create(newRemoteTargetUri.getPath());

            waitForRemoteTarget(remoteTargetRelativeUri);
            logger.d("Finished for Node: {}", nodeId);
        } catch (AssemblyException e) {
            logger.e("Remote target creation failed for Node: {}, error: {}, details: {}", nodeId, e.getErrorResponse(), e.getMessage());
            throw new RuntimeException(e);
        } catch (Exception e) {
            logger.e("Remote target creation failed for Node: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    @Override
    public UUID getServiceUuid() {
        // synchronize with random UUID,
        // cause creating logical drive can be long action and does not need synchronization
        return randomUUID();
    }

    private void waitForRemoteTarget(URI newRemoteTargetUri) throws TimeoutException, AssemblyException {
        logger.d("Waiting for remote target: {}", newRemoteTargetUri);

        // synchronize only discovery and attaching target to the node
        taskCoordinator.runThrowing(rssServiceUuid, ofMinutes(MINUTES_TO_WAIT_FOR_DISCOVERY), () -> {
            DiscoveryRunner discoveryRunner = beanFactory.create(DiscoveryRunner.class);
            discoveryRunner.setServiceUuid(rssServiceUuid);
            discoveryRunner.run();

            if (!targetAttacher.attachIfCompleted(nodeId, rssServiceUuid, newRemoteTargetUri)) {
                String message = format("Remote target(%s) cannot be attached to composed node(%s)", newRemoteTargetUri, nodeId);
                logger.e(message);
                throw new AssemblyException(message);
            }
        });
    }

    private URI sendLogicalDriveCreationRequest() throws AssemblyException, InterruptedException {
        LogicalDriveResourceCreationRequest request = buildLogicalDriveRequest();

        try (StorageServiceActions rssActions = actionsFactory.create(rssServiceUri)) {
            return rssActions.createBootableLogicalDrive(storageServiceSourceUri, request);
        } catch (ExternalServiceApiActionException e) {
            throw new AssemblyException("Logical Drive Creation Failed!", e.getErrorResponse(), e);
        }
    }

    private URI sendRemoteTargetCreationRequest(URI newLogicalDriveUri) throws AssemblyException {
        URI uri = create(newLogicalDriveUri.getPath());
        RemoteTargetResourceCreationRequest request = buildRemoteTargetRequest(uri);

        try (StorageServiceActions rssActions = actionsFactory.create(rssServiceUri)) {
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
        request.setInitiatorIqn("");
        request.setLunInfo(newLogicalDriveUri, requestedRemoteDrive.getIscsiAddress());

        return request;
    }
}
