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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.EventSubscriptionDto;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.redfish.json.templates.EventSubscriptionJson;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class EventSubscriptionDtoJsonSerializer extends DtoJsonSerializer<EventSubscriptionDto> {

    public EventSubscriptionDtoJsonSerializer() {
        super(EventSubscriptionDto.class);
    }

    @Override
    protected EventSubscriptionJson translate(EventSubscriptionDto dto) {
        EventSubscriptionJson eventSubscriptionJson = new EventSubscriptionJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        eventSubscriptionJson.oDataId = oDataId;
        eventSubscriptionJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        eventSubscriptionJson.destination = dto.getDestination();
        eventSubscriptionJson.protocol = dto.getProtocol();
        eventSubscriptionJson.description = dto.getDescription();
        eventSubscriptionJson.eventTypes = dto.getEventTypes();
        eventSubscriptionJson.id = dto.getId();
        eventSubscriptionJson.name = dto.getName();
        eventSubscriptionJson.originResources = dto.getOriginResources();
        eventSubscriptionJson.subscriptionContext = dto.getSubscriptionContext();
        return eventSubscriptionJson;
    }
}
