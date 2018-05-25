/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper;
import com.intel.podm.redfish.json.templates.ComposedNodeCollectionJson;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromODataType;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static com.intel.podm.redfish.serializers.CollectionTypeToCollectionODataMapping.getOdataForCollectionType;
import static java.util.stream.Collectors.toList;

@Dependent
public class ComposedNodeCollectionDtoJsonSerializer extends DtoJsonSerializer<ComposedNodeCollectionDto> {

    public ComposedNodeCollectionDtoJsonSerializer() {
        super(ComposedNodeCollectionDto.class);
    }

    @Override
    protected ComposedNodeCollectionJson translate(ComposedNodeCollectionDto composedNodeCollectionDto) {
        ODataForCollection oData = getOdataForCollectionType(composedNodeCollectionDto.getType());

        ComposedNodeCollectionJson json = new ComposedNodeCollectionJson(oData.getODataType());
        json.oDataContext = getContextFromODataType(oData.getODataType());
        json.oDataId = oDataIdFromUri(context.getRequestPath());
        json.name = oData.getName();
        json.members.addAll(composedNodeCollectionDto.getMembers().stream().map(ODataIdFromContextHelper::asOdataId).collect(toList()));
        json.actions.allocate.target = json.oDataId + "/Actions/Allocate";

        return json;
    }
}
