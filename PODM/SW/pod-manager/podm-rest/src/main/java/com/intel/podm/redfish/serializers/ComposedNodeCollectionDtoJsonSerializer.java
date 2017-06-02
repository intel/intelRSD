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

import com.intel.podm.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.podm.redfish.json.templates.ComposedNodeCollectionJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.stream.Collectors;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.appendOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class ComposedNodeCollectionDtoJsonSerializer extends DtoJsonSerializer<ComposedNodeCollectionDto> {

    public ComposedNodeCollectionDtoJsonSerializer() {
        super(ComposedNodeCollectionDto.class);
    }

    @Override
    protected ComposedNodeCollectionJson translate(ComposedNodeCollectionDto composedNodeCollectionDto) {
        ODataForCollection oData = CollectionTypeToCollectionODataMapping.getOdataForCollectionType(composedNodeCollectionDto.getType());

        ComposedNodeCollectionJson composedNodeCollectionJson = new ComposedNodeCollectionJson(oData.getODataType());

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        composedNodeCollectionJson.oDataContext = getContextFromId(oDataId);
        composedNodeCollectionJson.oDataId = oDataId;
        composedNodeCollectionJson.name = oData.getName();
        composedNodeCollectionJson.members.addAll(composedNodeCollectionDto.getMembers().stream()
            .map(id -> appendOdataId(composedNodeCollectionJson.oDataId, id))
            .collect(Collectors.toList()));
        composedNodeCollectionJson.actions.allocate.target = composedNodeCollectionJson.oDataId + "/Actions/Allocate";

        return composedNodeCollectionJson;
    }
}
