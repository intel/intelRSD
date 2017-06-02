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

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.WebClient;

import java.net.URI;

public class EventActions {
    private WebClient webClient;

    public EventActions(WebClient webClient) {
        this.webClient = webClient;
    }

    public URI subscribe(URI eventSubscribeUri, EventSubscribeRequest eventSubscribeRequest) throws ExternalServiceApiActionException {
        return webClient.post(eventSubscribeUri, eventSubscribeRequest);
    }

    public void deleteSubscription(URI subscriptionUri) throws ExternalServiceApiActionException {
        webClient.delete(subscriptionUri);
    }
}
