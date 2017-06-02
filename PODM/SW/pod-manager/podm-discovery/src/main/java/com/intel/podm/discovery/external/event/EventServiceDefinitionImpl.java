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

package com.intel.podm.discovery.external.event;

import com.intel.podm.client.api.events.EventServiceDefinition;

import java.net.URI;
import java.util.UUID;

public class EventServiceDefinitionImpl implements EventServiceDefinition {
    private final URI podmEventServiceDestinationUri;
    private final URI serviceBaseUri;
    private final UUID podmUuid;

    public EventServiceDefinitionImpl(URI podmEventServiceDestinationUri, URI serviceBaseUri, UUID podmUuid) {
        this.podmEventServiceDestinationUri = podmEventServiceDestinationUri;
        this.serviceBaseUri = serviceBaseUri;
        this.podmUuid = podmUuid;
    }

    @Override
    public URI getPodmEventServiceDestinationUri() {
        return podmEventServiceDestinationUri;
    }

    @Override
    public URI getServiceBaseUri() {
        return serviceBaseUri;
    }

    @Override
    public UUID getPodmUuid() {
        return podmUuid;
    }
}
