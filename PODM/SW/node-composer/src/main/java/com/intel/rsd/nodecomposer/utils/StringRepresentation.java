/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils;

import com.google.common.base.Joiner;
import com.google.common.base.Splitter;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public final class StringRepresentation {
    public static final String NULL_VALUE = "__NULL__";
    public static final String ELEMENT_SEPARATOR = ",";
    public static final String KEY_VALUE_SEPARATOR = "=";

    private StringRepresentation() {
    }

    public static Map<String, String> toMap(String stringToSplit) {
        return toMap(stringToSplit, false, false);
    }

    public static Map<String, String> toMap(String stringToSplit, boolean trimResults, boolean omitEmptyStrings) {
        Splitter splitter = Splitter.on(ELEMENT_SEPARATOR);

        if (trimResults) {
            splitter = splitter.trimResults();
        }

        if (omitEmptyStrings) {
            splitter = splitter.omitEmptyStrings();
        }

        Map<String, String> resultMap = splitter
                .withKeyValueSeparator(KEY_VALUE_SEPARATOR)
                .split(stringToSplit);
        resultMap = new LinkedHashMap<>(resultMap);
        convertStringNullValues(resultMap);
        return resultMap;
    }

    private static void convertStringNullValues(Map<String, String> map) {
        Iterator<Map.Entry<String, String>> it = map.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<String, String> element = it.next();
            if (NULL_VALUE.equals(element.getValue())) {
                element.setValue(null);
            }
        }
    }

    public static List<String> toList(String stringToSplit) {
        return toList(stringToSplit, false);
    }

    public static List<String> toList(String stringToSplit, boolean trimResults) {
        Splitter splitter = Splitter.on(ELEMENT_SEPARATOR);

        if (trimResults) {
            splitter = splitter.trimResults();
        }

        return splitter.splitToList(stringToSplit);
    }

    public static String fromMap(Map<?, ?> mapToJoin) {
        return Joiner
                .on(ELEMENT_SEPARATOR)
                .withKeyValueSeparator(KEY_VALUE_SEPARATOR)
                .useForNull(NULL_VALUE)
                .join(mapToJoin);
    }

    public static String fromIterable(Iterable<?> listToJoin) {
        return fromIterable(listToJoin, ELEMENT_SEPARATOR);
    }

    public static String fromIterable(Iterable<?> listToJoin, String separator) {
        return Joiner
                .on(separator)
                .join(listToJoin);
    }
}
