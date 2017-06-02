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

import com.intel.podm.business.dto.redfish.SubscriptionDto;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.redfish.json.templates.SubscriptionJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.Arrays;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class SubscriptionDtoJsonSerializer extends DtoJsonSerializer<SubscriptionDto> {
    protected SubscriptionDtoJsonSerializer() {
        super(SubscriptionDto.class);
    }

    @Override
    protected SubscriptionJson translate(SubscriptionDto dto) {
        SubscriptionJson simpleStorageJson = new SubscriptionJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        simpleStorageJson.oDataId = oDataId;
        simpleStorageJson.oDataContext = getContextFromId(oDataId);

        simpleStorageJson.id = dto.getId();
        simpleStorageJson.name = dto.getName();
        simpleStorageJson.context = dto.getContext();
        simpleStorageJson.protocol = dto.getProtocol();

        simpleStorageJson.eventTypes.addAll(Arrays.asList(EventType.values()));
        simpleStorageJson.destination = dto.getDestination();

        return simpleStorageJson;
    }
}
