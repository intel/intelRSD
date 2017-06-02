/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.dto.redfish.ZoneDto;
import com.intel.podm.redfish.json.templates.ZoneJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class ZoneDtoJsonSerializer extends BaseDtoJsonSerializer<ZoneDto> {
    public ZoneDtoJsonSerializer() {
        super(ZoneDto.class);
    }

    @Override
    protected ZoneJson translate(ZoneDto dto) {
        ZoneJson zoneJson = new ZoneJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        zoneJson.oDataId = oDataId;
        zoneJson.oDataContext = getContextFromId(oDataId);

        zoneJson.id = dto.getId();
        zoneJson.name = dto.getName();
        zoneJson.description = dto.getDescription();
        zoneJson.status = dto.getStatus();
        zoneJson.links.endpoints.addAll(asOdataIdSet(dto.getLinks().getEndpoints()));
        zoneJson.links.involvedSwitches .addAll(asOdataIdSet(dto.getLinks().getInvolvedSwitches()));
        return zoneJson;
    }
}
