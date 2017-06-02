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

package com.intel.podm.common.types;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;
import org.atteo.classindex.IndexSubclasses;

import java.util.List;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;
import static org.atteo.classindex.ClassIndex.getSubclasses;

/**
 * All enums which implement this interface will be automatically deserialized to String value got from method getValue();
 */
@IndexSubclasses
public interface EnumeratedType {
    Logger LOGGER = LoggerFactory.getLogger(EnumeratedType.class);

    Set<Class<? extends EnumeratedType>> SUB_TYPES = stream(getSubclasses(EnumeratedType.class).spliterator(), false).collect(toSet());

    String getValue();

    static <T extends EnumeratedType> T stringToEnum(Class<T> enumType, String text) {
        T foundEnumeration = findEnum(enumType, text);
        if (foundEnumeration == null) {
            LOGGER.w("No constant with text [{}] found for enum [{}]", text, enumType.getSimpleName());
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

    default List<String> getAliases() {
        return emptyList();
    }
}
