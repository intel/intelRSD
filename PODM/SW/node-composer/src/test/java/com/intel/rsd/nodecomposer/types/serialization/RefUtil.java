/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.serialization;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.types.Ref;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.google.common.base.Strings.isNullOrEmpty;
import static java.util.Collections.emptyList;

public final class RefUtil {
    private RefUtil() {
    }

    public static List<String> verifyRefField(Class<?> clazz, Field field) {
        // Use @JsonIgnore annotation over field when supplying @JsonSetter over method
        JsonIgnore jsonIgnore = field.getAnnotation(JsonIgnore.class);
        if (jsonIgnore != null) {
            return emptyList();
        }

        List<String> errors = new ArrayList<>();
        JsonSetter jsonSetter = field.getAnnotation(JsonSetter.class);
        if (!isJsonSetterAnnotationValid(jsonSetter)) {
            JsonProperty jsonProperty = field.getAnnotation(JsonProperty.class);
            String propertyName = getPropertyName(field, jsonProperty);

            errors.add(clazz.getName() + ": [" + field.getName() + "] field should be annotated:"
                + " @JsonSetter(value = \"" + propertyName + "\", nulls = AS_EMPTY)"
                + " due to use of " + Ref.class.getName());

            if (jsonProperty != null) {
                errors.add(clazz.getName() + ": [" + field.getName() + "] field needn't be annotated: @JsonProperty(...) - remove");
            }
        }

        return errors;
    }

    private static boolean isJsonSetterAnnotationValid(JsonSetter jsonSetter) {
        return jsonSetter != null && jsonSetter.nulls() == AS_EMPTY;
    }

    private static String getPropertyName(Field field, JsonProperty jsonProperty) {
        String name = field.getName();
        return isJsonPropertyDefined(jsonProperty) ? jsonProperty.value() : createPropertyFromFieldName(name);
    }

    private static boolean isJsonPropertyDefined(JsonProperty jsonProperty) {
        return jsonProperty != null && !isNullOrEmpty(jsonProperty.value());
    }

    private static String createPropertyFromFieldName(String name) {
        return name.substring(0, 1).toUpperCase() + name.substring(1);
    }

}
