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

package com.intel.rsd.nodecomposer.rest.redfish.resources;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.nodecomposer.discovery.external.events.VolumeCreatedEvent;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationEventPublisher;
import org.springframework.context.annotation.Bean;
import org.springframework.context.event.ApplicationEventMulticaster;
import org.springframework.context.event.SimpleApplicationEventMulticaster;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.net.URI;
import java.util.Objects;
import java.util.concurrent.Executors;

@RestController
@RequestMapping(path = "/EventReceiver")
@Slf4j
public class EventReceivingEndpoint {

    private final boolean eventProcessingEnabled;
    private final ApplicationEventPublisher applicationEventPublisher;

    public EventReceivingEndpoint(@Value("${event_processing.enabled:false}") boolean eventProcessingEnabled,
                                  ApplicationEventPublisher applicationEventPublisher) {
        this.eventProcessingEnabled = eventProcessingEnabled;
        this.applicationEventPublisher = applicationEventPublisher;
    }

    @PostMapping
    public void eventReceiver(@RequestBody JsonNode jsonNode) {
        if (!eventProcessingEnabled) {
            return;
        }
        log.debug("Received event: {}", jsonNode);
        for (JsonNode event : jsonNode.withArray("Events")) {
            String eventType = event.at("/EventType").asText();
            String originOfCondition = event.at("/OriginOfCondition/@odata.id").asText();
            if (isVolumeCreationEvent(eventType, originOfCondition)) {
                applicationEventPublisher.publishEvent(new VolumeCreatedEvent(this, URI.create(originOfCondition)));
            }
        }
    }

    private boolean isVolumeCreationEvent(String eventType, @NonNull String originOfCondition) {
        return Objects.equals(eventType, "ResourceAdded")
            && originOfCondition.matches("^\\/redfish\\/v1\\/StorageServices\\/[A-Za-z0-9-.%:+_]+\\/Volumes\\/[A-Za-z0-9-.%:+_]+$");
    }

    @Bean
    ApplicationEventMulticaster applicationEventMulticaster(@Value("${event_processing.thread.pool.size:5}") int threadPoolSize) {
        val applicationEventMulticaster = new SimpleApplicationEventMulticaster();
        applicationEventMulticaster.setTaskExecutor(Executors.newFixedThreadPool(threadPoolSize));
        return applicationEventMulticaster;
    }

}
