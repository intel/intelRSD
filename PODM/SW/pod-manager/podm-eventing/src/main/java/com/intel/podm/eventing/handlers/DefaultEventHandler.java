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

package com.intel.podm.eventing.handlers;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.DiscoveryScheduler;
import com.intel.podm.eventing.dto.EventArrayDto;
import com.intel.podm.eventing.dto.EventDto;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class DefaultEventHandler implements EventHandler {

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private Logger logger;

    @Override
    public void handle(EventArrayDto eventArrayDto) {
        //FIXME: get rid of this
        for (EventDto eventDto : eventArrayDto.getEvents()) {
            if (eventDto.getOriginOfCondition().contains("Ports")) {
                return;
            }
        }
        logger.d("Will trigger discovery as a result of processing event: {}", eventArrayDto);
        discoveryScheduler.triggerDiscovery(eventArrayDto.getServiceUuid());
    }
}
