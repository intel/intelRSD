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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.EventServiceDto;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.redfish.json.templates.EventServiceJson;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class EventServiceDtoJsonSerializer extends DtoJsonSerializer<EventServiceDto> {

    public EventServiceDtoJsonSerializer() {
        super(EventServiceDto.class);
    }

    @Override
    protected EventServiceJson translate(EventServiceDto dto) {
        EventServiceJson eventServiceJson = new EventServiceJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        eventServiceJson.oDataId = oDataId;
        eventServiceJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        eventServiceJson.deliveryRetryAttempts = dto.getDeliveryRetryAttempts();
        eventServiceJson.deliveryRetryIntervalSeconds = dto.getDeliveryRetryIntervalSeconds();
        eventServiceJson.eventTypesForSubscription = dto.getEventTypesForSubscription();
        eventServiceJson.description = dto.getDescription();
        eventServiceJson.id = dto.getId();
        eventServiceJson.name = dto.getName();
        eventServiceJson.serviceEnabled = dto.getServiceEnabled();
        eventServiceJson.status = dto.getStatus();
        eventServiceJson.subscriptions = dto.getSubscriptions();
        return eventServiceJson;
    }
}
