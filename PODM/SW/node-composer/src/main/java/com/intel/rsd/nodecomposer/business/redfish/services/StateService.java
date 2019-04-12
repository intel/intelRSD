/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.VolumeResource;
import com.intel.rsd.nodecomposer.types.State;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.State.ENABLED;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class StateService {
    @Value("${external-service.check-status-after-volume-creation-delay-millis:5000}")
    private long checkStatusAfterVolumeCreationDelayMillis;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public StateService(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    public void waitForEnabledStateOnVolume(URI volumeUri) {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            waitForEnabledState(webClient, volumeUri);
        } catch (InterruptedException e) {
            log.error("Waiting for ENABLED state failed for Volume: {}, details: {}", volumeUri, e.getMessage());
            Thread.currentThread().interrupt();
        }
    }

    private void waitForEnabledState(WebClient webClient, URI resourceUri) throws InterruptedException {
        URI path = URI.create(resourceUri.getPath());
        while (true) {
            Optional<State> volumeState = readVolumeState(webClient, path);

            if (volumeState.isPresent()) {
                if (ENABLED.equals(volumeState.get())) {
                    return;
                }
            }
            Thread.sleep(checkStatusAfterVolumeCreationDelayMillis);
        }
    }

    private Optional<State> readVolumeState(WebClient webClient, URI path) {
        try {
            VolumeResource volume = (VolumeResource) webClient.get(path);
            return ofNullable(volume.getStatus().getState());
        } catch (WebClientRequestException e) {
            return empty();
        }
    }
}
