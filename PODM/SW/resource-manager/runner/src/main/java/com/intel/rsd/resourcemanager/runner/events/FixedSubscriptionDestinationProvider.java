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

package com.intel.rsd.resourcemanager.runner.events;

import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.stereotype.Component;

import java.net.URI;

import static com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.ReceiverType.Fixed;

@ConditionalOnProperty(name = "events.receiver.type", havingValue = "Fixed", matchIfMissing = true)
@Component
@Slf4j
class FixedSubscriptionDestinationProvider implements SubscriptionDestinationProvider {
    private final URI destination;

    @Autowired
    FixedSubscriptionDestinationProvider(EventsConfiguration eventsConfiguration) {
        destination = eventsConfiguration.getReceiver().getFixed().getTargetUri();
        log.info("Starting Receiver with {} configuration: {}", Fixed, eventsConfiguration.getReceiver());
    }

    @Override
    public URI provideUriFor(URI serviceUri) {
        return destination;
    }
}
