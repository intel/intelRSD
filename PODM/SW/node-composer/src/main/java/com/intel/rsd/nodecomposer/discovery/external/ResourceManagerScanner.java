/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraph;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraphBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import com.intel.rsd.services.common.hazelcast.ScheduledWithFixedDelayOnLeaderTask;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationEventPublisher;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.time.Duration;

import static com.intel.rsd.nodecomposer.discovery.external.DiscoveryEvent.EventType.FINISHED;
import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.DISCOVERY_TASK_SCHEDULER;

@Slf4j
@Component
public class ResourceManagerScanner extends ScheduledWithFixedDelayOnLeaderTask {
    private final ApplicationEventPublisher applicationEventPublisher;
    private final RestGraphBuilder restGraphBuilder;
    private final RestGraphPersister restGraphPersister;
    private final Duration discoveryIntervalSeconds;

    @Autowired
    public ResourceManagerScanner(@Qualifier(DISCOVERY_TASK_SCHEDULER) TaskScheduler taskScheduler,
                                  @Value("${discovery.interval-seconds:600}") long discoveryIntervalSeconds,
                                  ApplicationEventPublisher applicationEventPublisher,
                                  RestGraphBuilder restGraphBuilder,
                                  RestGraphPersister restGraphPersister) {
        super(taskScheduler);
        this.applicationEventPublisher = applicationEventPublisher;
        this.discoveryIntervalSeconds = Duration.ofSeconds(discoveryIntervalSeconds);
        this.restGraphBuilder = restGraphBuilder;
        this.restGraphPersister = restGraphPersister;
    }

    @Override
    @SuppressWarnings("checkstyle:IllegalCatch")
    @TimeMeasured(tag = "[DiscoveryTask]")
    public void run() {
        try {
            RestGraph restGraph = restGraphBuilder.build();
            restGraphPersister.persist(restGraph);
        } catch (WebClientRequestException e) {
            log.error("Problem while reading: {}: {}: {}", e.getResourceUri(), e.getMessage(), e.getCause().getMessage());
        } catch (RuntimeException e) {
            log.warn("Unable to process resource-manager data: {}, {}", e.getMessage(), e.getCause());
        } finally {
            applicationEventPublisher.publishEvent(new DiscoveryEvent(FINISHED));
        }
    }

    @Override
    protected Duration getFixedDelay() {
        return discoveryIntervalSeconds;
    }

    @Override
    public String toString() {
        return "resource-manager-scanner";
    }
}


