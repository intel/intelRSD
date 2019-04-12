/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.common;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.val;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static java.util.stream.Collectors.toMap;

/**
 * This class is expected to run only on single thread, so it's not thread safe
 */
public class MapUpdater {
    public static <A, B> void updateValues(Map<A, B> currentValues, Map<A, B> valuesFromResponse) {
        val positiveDelta = getAddedEntries(currentValues, valuesFromResponse);
        val negativeDelta = getRemovedEntries(currentValues, valuesFromResponse);
        val intersection = getIntersectingEntriesWithUpdatedValues(currentValues, valuesFromResponse);
        negativeDelta.forEach(currentValues::remove);
        currentValues.putAll(positiveDelta);
        currentValues.putAll(intersection);
    }

    private static <A, B> Map<A, B> getIntersectingEntriesWithUpdatedValues(Map<A, B> currentValues, Map<A, B> valuesFromResponse) {
        val result = new HashMap<A, B>();
        for (Entry<A, B> entry : valuesFromResponse.entrySet()) {
            if (currentValues.containsKey(entry.getKey())) {
                result.put(entry.getKey(), entry.getValue());
            }
        }
        return result;
    }

    private static <A, B> Set<A> getRemovedEntries(Map<A, B> currentValues, Map<A, B> valuesFromResponse) {
        Iterator<Map.Entry<A, B>> it = currentValues.entrySet().iterator();
        Set<A> difference = new HashSet<>();
        while (it.hasNext()) {
            Map.Entry<A, B> entry = it.next();
            if (!valuesFromResponse.containsKey(entry.getKey())) {
                difference.add(entry.getKey());
            }
        }
        return difference;
    }

    private static <A, B> Map<A, B> getAddedEntries(Map<A, B> valuesFromCache, Map<A, B> valuesFromResponse) {
        Iterator<Map.Entry<A, B>> it = valuesFromResponse.entrySet().iterator();
        Map<A, B> difference = new HashMap<>();
        while (it.hasNext()) {
            Entry<A, B> entry = it.next();
            if (!valuesFromCache.containsKey(entry.getKey())) {
                difference.put(entry.getKey(), entry.getValue());
            }
        }
        return difference;
    }

    public static Map<String, String> convertToStringStringMap(Map<String, JsonNode> map) {
        return map.entrySet().stream().collect(toMap(Entry::getKey, entry -> entry.getValue().toString()));
    }

    public static Map<String, JsonNode> convertToStringJsonNodeMap(Map<String, String> map) {
        return map.entrySet().stream().collect(toMap(Entry::getKey, entry -> stringToJsonNode(entry.getValue())));
    }
}
