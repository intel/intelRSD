/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.entities.base;

import com.google.common.reflect.TypeToken;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import static com.google.common.base.Preconditions.checkNotNull;
import static java.lang.reflect.Modifier.isStatic;
import static java.util.Collections.unmodifiableMap;

public final class DomainObjectInspector {
    private final Class<? extends DomainObject> type;

    private DomainObjectInspector(Class<? extends DomainObject> type) {
        this.type = checkNotNull(type);
    }

    public static DomainObjectInspector inspect(Class<? extends DomainObject> type) {
        return new DomainObjectInspector(type);
    }

    public Map<String, Class> getProperties() {
        HashMap<String, Class> result = new HashMap<>();

        for (DomainObjectProperty p : getDomainObjectProperties()) {
            result.put(p.getName(), p.getType());
        }

        return unmodifiableMap(result);
    }

    private Iterable<DomainObjectProperty> getDomainObjectProperties() {
        List<DomainObjectProperty> result = new ArrayList<>();

        for (Field field : getAllStaticFields()) {
            DomainObjectProperty p = toProperty(field);
            if (p != null) {
                result.add(p);
            }
        }

        return result;
    }

    private Iterable<Field> getAllStaticFields() {
        HashSet<Field> result = new HashSet<>();

        for (TypeToken token : TypeToken.of(type).getTypes()) {
            for (Field f : token.getRawType().getDeclaredFields()) {
                if (isStatic(f.getModifiers())) {
                    result.add(f);
                }
            }
        }

        return result;
    }


    private DomainObjectProperty toProperty(Field field) {
        Object property;
        try {
            property = field.get(null);
        } catch (IllegalAccessException e) {
            // fields declared by Proxy classes will be skipped
            return null;
        }

        return property instanceof DomainObjectProperty
                ? (DomainObjectProperty) property
                : null;
    }
}
