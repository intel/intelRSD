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

import com.intel.podm.business.dto.redfish.FabricDto;
import com.intel.podm.redfish.json.templates.FabricJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

public class FabricDtoJsonSerializer extends BaseDtoJsonSerializer<FabricDto> {
    protected FabricDtoJsonSerializer() {
        super(FabricDto.class);
    }

    @Override
    protected FabricJson translate(FabricDto dto) {

        FabricJson fabricJson = new FabricJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        fabricJson.oDataId = oDataId;
        fabricJson.oDataContext = getContextFromId(oDataId);

        fabricJson.id = dto.getId();
        fabricJson.name = dto.getName();
        fabricJson.fabricType = dto.getFabricType();
        fabricJson.description = dto.getDescription();
        fabricJson.maxZones = dto.getMaxZones();
        fabricJson.status = dto.getStatus();
        fabricJson.zones = oDataIdFromUri(create(oDataId + "/Zones"));
        fabricJson.endpoints = oDataIdFromUri(create(oDataId + "/Endpoints"));
        fabricJson.switches = oDataIdFromUri(create(oDataId + "/Switches"));
        return fabricJson;
    }
}
