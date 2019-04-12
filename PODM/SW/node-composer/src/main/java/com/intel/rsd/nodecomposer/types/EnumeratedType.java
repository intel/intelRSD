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

package com.intel.rsd.nodecomposer.types;

import org.slf4j.Logger;

import java.util.Comparator;
import java.util.List;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static java.util.Collections.emptyList;
import static java.util.Comparator.comparing;
import static org.slf4j.LoggerFactory.getLogger;

/**
 * All enums which implement this interface will be automatically deserialized to String value got from method getValue();
 */
@AutoDeserializing
public interface EnumeratedType {
    Logger LOGGER = getLogger(EnumeratedType.class);
    // Priority defines order of enums. Default value is 1 - this is the highest priority enum can have.
    // Priority is mainly used in allocation algorithms to specify default selection of resources.
    Integer DEFAULT_PRIORITY = 1;

    static <T extends EnumeratedType> T stringToEnum(Class<T> enumType, String text) {
        T foundEnumeration = findEnum(enumType, text);
        if (foundEnumeration == null) {
            LOGGER.warn("No constant with text [{}] found for enum [{}]", text, enumType.getSimpleName());
        }

        return foundEnumeration;
    }

    static <T extends EnumeratedType> T stringToEnumStrictly(Class<T> enumType, String text) {
        T foundEnumeration = findEnum(enumType, text);
        if (foundEnumeration == null) {
            throw new IllegalArgumentException("No constant with text [" + text + "] found for enum ["
                + enumType.getSimpleName() + "]");
        }

        return foundEnumeration;
    }

    static <T extends EnumeratedType> T findEnum(Class<T> enumType, String text) {
        requires(enumType != null, "Type of enum (class) should not be null");
        requires(text != null, "Constant with null test for enum [" + enumType.getSimpleName() + "] is not allowed");

        for (T extension : enumType.getEnumConstants()) {
            if (text.equalsIgnoreCase(extension.getValue()) || extension.getAliases().contains(text)) {
                return extension;
            }
        }
        return null;
    }

    static <T extends EnumeratedType> Comparator<T> byPriority() {
        return comparing(T::getPriority);
    }

    String getValue();
    default List<String> getAliases() {
        return emptyList();
    }
    default Integer getPriority() {
        return DEFAULT_PRIORITY;
    }
}
