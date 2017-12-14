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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.podm.client.resources.redfish.UnknownOemObject;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.intel.podm.common.types.redfish.OemType.KNOWN_OEMS;
import static com.intel.podm.common.types.redfish.OemType.OEM_ELEMENT_SEPARATOR;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static java.util.Collections.emptyList;
import static java.util.Optional.empty;
import static java.util.Optional.of;

public final class UnknownOemsHelper {
    private static final ObjectMapper OBJECT_MAPPER = new ObjectMapper();

    private UnknownOemsHelper() {
    }

    public static Optional<UnknownOemObject> createUnknownOemObject(List<String> oemNodePath, String oemName, JsonNode oemValue)
        throws JsonProcessingException {
        if (oemValue == null || KNOWN_OEMS.contains(oemName)) {
            return empty();
        }
        List<String> copyOfOemNodePath = new ArrayList<>(oemNodePath);
        copyOfOemNodePath.add(oemName);

        return of(new UnknownOemObject(fromIterable(copyOfOemNodePath, OEM_ELEMENT_SEPARATOR), convertJsonNodeToString(oemValue)));
    }

    public static Collection<UnknownOemObject> extractUnknownOemsFromOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
        return extractUnknownOems(TOP_LEVEL_OEM.getPath(), oemName, oemValue);
    }

    public static Collection<UnknownOemObject> extractUnknownOemsFromLinksOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
        return extractUnknownOems(OEM_IN_LINKS.getPath(), oemName, oemValue);
    }

    public static Collection<UnknownOemObject> extractUnknownOemsFromActionsOemObject(String oemName, JsonNode oemValue) throws JsonProcessingException {
        return extractUnknownOems(OEM_IN_ACTIONS.getPath(), oemName, oemValue);
    }

    public static <T> T convertObjectNodeToObject(ObjectNode oemValue, Class<T> valueType) throws JsonProcessingException {
        return OBJECT_MAPPER.treeToValue(oemValue, valueType);
    }

    public static String convertJsonNodeToString(JsonNode oemValue) throws JsonProcessingException {
        if (oemValue == null) {
            return null;
        }

        return OBJECT_MAPPER.writeValueAsString(oemValue);
    }

    private static Collection<UnknownOemObject> extractUnknownOems(List<String> oemNodePath, String oemName, JsonNode oemValue)
        throws JsonProcessingException {
        if (!oemName.equals("Oem") || oemValue == null || !oemValue.isObject()) {
            return emptyList();
        }

        List<UnknownOemObject> unknownOemObjects = new ArrayList<>();
        Iterator<Map.Entry<String, JsonNode>> childIterator = oemValue.fields();
        while (childIterator.hasNext()) {
            Map.Entry<String, JsonNode> childEntry = childIterator.next();
            if (childEntry.getValue().isObject()) {
                createUnknownOemObject(oemNodePath, childEntry.getKey(), childEntry.getValue())
                    .ifPresent(unknownOemObjects::add);
            }
        }

        return unknownOemObjects;
    }
}
