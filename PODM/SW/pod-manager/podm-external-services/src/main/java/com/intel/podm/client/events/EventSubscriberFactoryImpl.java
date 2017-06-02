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

package com.intel.podm.client.events;

import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.api.events.EventServiceDefinition;
import com.intel.podm.client.api.events.EventSubscriberFactory;
import com.intel.podm.client.api.events.EventSubscriptionManager;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Instance;
import javax.enterprise.inject.New;
import javax.inject.Inject;
import java.net.URI;

import static java.net.URI.create;

@Dependent
public class EventSubscriberFactoryImpl implements EventSubscriberFactory {

    @Inject
    @New
    private Instance<EventSubscriptionManagerImpl> eventSubscriberFactory;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Override
    public EventSubscriptionManager createEventSubscriberManager(EventServiceDefinition eventServiceDefinition) {
        EventSubscriptionManagerImpl eventSubscriber = eventSubscriberFactory.get();
        URI baseUri = create(eventServiceDefinition.getServiceBaseUri().toString());
        eventSubscriber.setWebClient(webClientBuilder.newInstance(baseUri).retryable().build());
        eventSubscriber.setEventServiceDefinition(eventServiceDefinition);
        return eventSubscriber;
    }

}
