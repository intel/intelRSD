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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.redfish.services.assembly.AssemblyException;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.LogicalDriveResourceCreationRequest;
import com.intel.podm.client.actions.RemoteTargetResourceCreationRequest;
import com.intel.podm.client.resources.redfish.LogicalDriveResource;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.State;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ExternalServiceConfig;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.types.ReplicationMethod.SNAPSHOT;
import static java.lang.String.format;
import static java.time.Duration.ofMinutes;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static java.util.UUID.randomUUID;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class NewRemoteDriveAssemblyTask extends NodeTask {
    private static final int MINUTES_TO_WAIT_FOR_DISCOVERY = 5;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Inject
    private TargetAttacher targetAttacher;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private ServiceExplorer serviceExplorer;

    @Inject
    @Config
    private Holder<ExternalServiceConfig> configHolder;

    private URI masterDriveSourceUri;
    private RequestedNode.RemoteDrive requestedRemoteDrive;
    private URI rssServiceUri;
    private URI storageServiceSourceUri;
    private URI logicalVolumeGroupSourceUri;
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

    public void setLogicalVolumeGroupSourceUri(URI logicalVolumeGroupSourceUri) {
        this.logicalVolumeGroupSourceUri = logicalVolumeGroupSourceUri;
    }

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    public void run() {
        logger.d("Running for Node: {}", nodeId);

        try {
            URI newLogicalDriveUri = sendLogicalDriveCreationRequest();
            URI newRemoteTargetUri = sendRemoteTargetCreationRequest(newLogicalDriveUri);
            URI remoteTargetRelativeUri = URI.create(newRemoteTargetUri.getPath());

            waitForRemoteTarget(remoteTargetRelativeUri);
            logger.d("Finished for Node: {}", nodeId);
        } catch (AssemblyException e) {
            logger.e(format("Remote target creation failed for Node: %s", nodeId), e);
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
        taskCoordinator.run(rssServiceUuid, ofMinutes(MINUTES_TO_WAIT_FOR_DISCOVERY), () -> {
            serviceExplorer.discover(rssServiceUuid);

            if (!targetAttacher.attachIfCompleted(nodeId, rssServiceUuid, newRemoteTargetUri)) {
                String message = format("Remote target(%s) cannot be attached to composed node(%s)", newRemoteTargetUri, nodeId);
                logger.e(message);
                throw new AssemblyException(message);
            }
        });
    }

    private URI sendLogicalDriveCreationRequest() throws AssemblyException, InterruptedException {
        LogicalDriveResourceCreationRequest request = buildLogicalDriveRequest();

        try (WebClient webClient = webClientBuilder.newInstance(rssServiceUri).retryable().build()) {
            LogicalDriveResource logicalDrive = getNewlyCreatedLogicalDrive(request, webClient);
            if (!logicalDrive.isBootable()) {
                throw new AssemblyException("Logical Drive was created but is not bootable.");
            }
            return URI.create(logicalDrive.getUri().getPath());
        } catch (WebClientRequestException e) {
            throw new AssemblyException("Logical Drive Creation Failed!", e);
        }
    }

    private LogicalDriveResource getNewlyCreatedLogicalDrive(LogicalDriveResourceCreationRequest request, WebClient webClient)
        throws WebClientRequestException, InterruptedException {
        URI targetUri = URI.create(storageServiceSourceUri.toString() + "/LogicalDrives");
        URI logicalDriveUri = webClient.post(targetUri, request);

        waitForEnabledState(webClient, logicalDriveUri);

        try {
            URI relativeLogicalDriveUri = URI.create(logicalDriveUri.getPath());
            return (LogicalDriveResource) webClient.get(relativeLogicalDriveUri);
        } catch (WebClientRequestException e) {
            throw new RuntimeException(e);
        }
    }

    private void waitForEnabledState(WebClient webClient, URI resourceUri) throws WebClientRequestException, InterruptedException {
        URI path = URI.create(resourceUri.getPath());

        while (true) {
            Optional<State> driveState = tryReadingDriveState(webClient, path);

            if (driveState.isPresent()) {
                switch (driveState.get()) {
                    case ENABLED:
                        return;
                    case ABSENT:
                        throw new WebClientRequestException("Error occurred during creation of logical drive", resourceUri, null);
                    default:
                        break;
                }
            }

            Thread.sleep(configHolder.get().getCheckStatusAfterLogicalDriveCreationDelayMillis());
        }
    }

    private Optional<State> tryReadingDriveState(WebClient webClient, URI path) {
        try {
            LogicalDriveResource drive = (LogicalDriveResource) webClient.get(path);
            return ofNullable(drive.getStatus().getState());
        } catch (WebClientRequestException e) {
            return empty();
        }
    }

    private URI sendRemoteTargetCreationRequest(URI newLogicalDriveUri) throws AssemblyException {
        URI uri = URI.create(newLogicalDriveUri.getPath());
        RemoteTargetResourceCreationRequest request = buildRemoteTargetRequest(uri);

        try (WebClient webClient = webClientBuilder.newInstance(rssServiceUri).retryable().build()) {
            URI targetUri = URI.create(storageServiceSourceUri.toString() + "/Targets");
            return webClient.post(targetUri, request);
        } catch (WebClientRequestException e) {
            throw new AssemblyException("Remote Target Creation Failed!", e);
        }
    }

    private LogicalDriveResourceCreationRequest buildLogicalDriveRequest() {
        LogicalDriveResourceCreationRequest request = new LogicalDriveResourceCreationRequest();
        request.setSnapshot(requestedRemoteDrive.getMaster().getType() == SNAPSHOT);
        request.setCapacityGib(requestedRemoteDrive.getCapacityGib());
        request.setMaster(masterDriveSourceUri);
        request.setLogicalVolumeGroupUri(logicalVolumeGroupSourceUri);

        return request;
    }

    private RemoteTargetResourceCreationRequest buildRemoteTargetRequest(URI newLogicalDriveUri) {
        RemoteTargetResourceCreationRequest request = new RemoteTargetResourceCreationRequest();
        request.setInitiatorIqn("");
        request.setLunInfo(newLogicalDriveUri, requestedRemoteDrive.getIscsiAddress());

        return request;
    }
}
