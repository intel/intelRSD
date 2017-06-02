/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EventServiceDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.EventsConfig;

import javax.inject.Inject;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toList;

public class EventServiceReaderServiceImpl implements ReaderService<EventServiceDto> {

    private EventsConfig eventsConfig;

    @Inject
    public EventServiceReaderServiceImpl(@Config Holder<EventsConfig> eventsConfig) {
        this.eventsConfig = eventsConfig.get();
    }

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        throw new UnsupportedOperationException("EventService is a singleton resource!");
    }

    @Override
    public EventServiceDto getResource(Context context) throws ContextResolvingException {
        // TODO: hardcode - to be changed.
        return EventServiceDto.newBuilder()
            .id("EventService")
            .name("EventService")
            .description("EventService")
            .serviceEnabled(true)
            .deliveryRetryAttempts(eventsConfig.getNorthboundConfiguration().getDeliveryRetryAttempts())
            .deliveryRetryIntervalSeconds(eventsConfig.getNorthboundConfiguration().getDeliveryRetryIntervalSeconds())
            .eventTypesForSubscription(stream(EventType.values()).collect(toList()))
            .status(Status.fromString("State=Enabled,Health=OK"))
            .subscriptions(oDataIdFromUri(create(asOdataId(context) + "/Subscriptions"))).build();
    }
}
