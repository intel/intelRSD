/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks.volume;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.redfish.VolumeResource;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.State;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ExternalServiceConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Optional;
import java.util.UUID;

import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static java.util.UUID.randomUUID;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class WaitForVolumeTask extends NewVolumeTask {

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;
    @Inject
    @Config
    private Holder<ExternalServiceConfig> configHolder;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        ExternalService rssService = getStorageServiceFromResourceDescriptor().getService();
        try (WebClient webClient = webClientBuilder.newInstance(rssService.getBaseUri()).retryable().build()) {
            waitForEnabledState(webClient, getNewRemoteVolumeUri());
        } catch (InterruptedException e) {
            logger.e("Remote target creation failed for Node: {}, details: {}", nodeId, e.getMessage());
        }
    }

    @Override
    public UUID getServiceUuid() {
        // synchronize with random UUID,
        // cause creating volume can be long action and does not need synchronization
        return randomUUID();
    }

    private void waitForEnabledState(WebClient webClient, URI resourceUri) throws InterruptedException {
        URI path = URI.create(resourceUri.getPath());
        while (true) {
            Optional<State> volumeState = tryReadingVolumeState(webClient, path);
            if (volumeState.isPresent()) {
                switch (volumeState.get()) {
                    case ENABLED:
                        return;
                    case ABSENT:
                        throw new RuntimeException(format("Error occurred during creation of volume on uri: %s", resourceUri));
                    default:
                        break;
                }
            }
            Thread.sleep(configHolder.get().getCheckStatusAfterVolumeCreationDelayMillis());
        }
    }

    private Optional<State> tryReadingVolumeState(WebClient webClient, URI path) {
        try {
            VolumeResource volume = (VolumeResource) webClient.get(path);
            return ofNullable(volume.getStatus().getState());
        } catch (WebClientRequestException e) {
            return empty();
        }
    }
}
