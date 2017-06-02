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

package com.intel.podm.redfish;

import com.fasterxml.jackson.databind.JsonNode;

import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import static com.intel.podm.common.types.redfish.OemType.OEM_ELEMENT_SEPARATOR;
import static com.intel.podm.common.utils.StringRepresentation.fromIterable;

public final class UnknownOemsJsonHelper {
    private UnknownOemsJsonHelper() { }

    public static Map<String, Object> createUnknownsOemsMapToAddFromOemPathAndAllUnknownOems(List<String> oemNodePath, Map<String, JsonNode> unknownOems) {
        Map<String, Object> unknownOemsToAdd = new LinkedHashMap<>();

        String oemNodePathBeginning = fromIterable(oemNodePath, OEM_ELEMENT_SEPARATOR);

        for (Map.Entry<String, JsonNode> unknownOemsEntry : unknownOems.entrySet()) {
            String unknownOemNodePath = unknownOemsEntry.getKey();
            JsonNode unknownOemValue = unknownOemsEntry.getValue();
            if (unknownOemNodePath.startsWith(oemNodePathBeginning + OEM_ELEMENT_SEPARATOR)) {
                String shortOemNodePath = unknownOemNodePath.replaceFirst(oemNodePathBeginning + OEM_ELEMENT_SEPARATOR, "");
                unknownOemsToAdd.put(shortOemNodePath, unknownOemValue);
            }
        }

        return unknownOemsToAdd;
    }
}
