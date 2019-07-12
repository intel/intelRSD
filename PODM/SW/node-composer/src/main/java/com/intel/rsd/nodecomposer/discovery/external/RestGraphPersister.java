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

import com.intel.rsd.nodecomposer.ModelState;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraph;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationEventPublisher;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.discovery.external.DiscoveryEvent.EventType.RETRY_REQUESTED;

@Component
@Slf4j
public class RestGraphPersister {
    private final ApplicationEventPublisher applicationEventPublisher;
    private final EntityGraphMapper mapper;
    private final ModelState modelState;

    @Autowired
    public RestGraphPersister(ApplicationEventPublisher applicationEventPublisher, EntityGraphMapper mapper, ModelState modelState) {
        this.applicationEventPublisher = applicationEventPublisher;
        this.mapper = mapper;
        this.modelState = modelState;
    }

    public void persist(RestGraph restGraph) {
        if (modelState.isNewerThan(restGraph.getCaptureTime())) {
            log.info("Model has been changed in the meantime, rescheduling discovery...");
            applicationEventPublisher.publishEvent(new DiscoveryEvent(RETRY_REQUESTED));
        } else {
            mapper.map(restGraph);
        }
    }
}
