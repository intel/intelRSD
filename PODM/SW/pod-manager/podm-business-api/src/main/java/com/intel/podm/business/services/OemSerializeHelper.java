/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.services;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.List;
import java.util.Map;

import static com.intel.podm.business.errors.ExtendedInfoDto.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toMap;

public final class OemSerializeHelper {
    private static final String ODATA_ID = "@odata.id";
    private static final String ODATA_ID_EXTENDED_INFO = ODATA_ID + "@Message.ExtendedInfo";

    private OemSerializeHelper() {
    }

    public static Map<String, JsonNode> oemDtosToUnknownOems(List<UnknownOemDto> unknownOemDtos) {
        return unknownOemDtos.stream().collect(toMap(UnknownOemDto::getOemPath, OemSerializeHelper::oemDtoToJsonNode));
    }

    public static JsonNode oemDtoToJsonNode(UnknownOemDto unknownOemDto) {
        return replaceOdataIds(unknownOemDto.getOemValue(), unknownOemDto.getFoundContexts());
    }

    private static JsonNode replaceOdataIds(JsonNode jsonNode, Map<String, Context> contexts) {
        JsonNode oDataIdJsonNode = jsonNode.get(ODATA_ID);
        if (oDataIdJsonNode != null && oDataIdJsonNode.isTextual()) {
            String sourceUriString = oDataIdJsonNode.asText();
            Context context = contexts.get(sourceUriString);
            ODataId oDataId = asOdataId(context);

            ObjectNode parentObjectNode = (ObjectNode) jsonNode;
            if (oDataId == null) {
                parentObjectNode.put(ODATA_ID, (String) null);
                parentObjectNode.putPOJO(
                    ODATA_ID_EXTENDED_INFO,
                    singletonList(newExtendedInfoBuilder("Could not convert " + ODATA_ID + " discovered from External Service.").build())
                );
            } else {
                parentObjectNode.put(ODATA_ID, oDataId.toString());
            }
        }

        for (JsonNode childNode : jsonNode) {
            replaceOdataIds(childNode, contexts);
        }

        return jsonNode;
    }
}
