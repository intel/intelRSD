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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.Entity;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;

@SuppressWarnings({"checkstyle:MethodCount"})
final class EntityRelationsHelper {
    static final String GETTER_PREFIX = "get";
    static final String SETTER_PREFIX = "set";
    static final String ADDER_PREFIX = "add";
    static final String UNLINKER_PREFIX = "unlink";

    private EntityRelationsHelper() {
    }

    @SuppressWarnings("unchecked")
    static <T> Class<T> unproxy(Class<? extends T> proxiedClass) {
        String name = proxiedClass.getSimpleName();

        if (name.contains("$")) {
            return (Class<T>) proxiedClass.getSuperclass();
        }

        return (Class<T>) proxiedClass;
    }

    @SuppressWarnings("unchecked")
    static Class<? extends Entity> getCollectionFieldContentType(Field collectionField) {
        return (Class<? extends Entity>) ((ParameterizedType) collectionField.getGenericType()).getActualTypeArguments()[0];
    }

    @SuppressWarnings("unchecked")
    static Class<? extends Entity> getExactlyOneEntityTypeFromMethodArguments(Method method) {
        if (method.getParameterTypes().length == 1 && Entity.class.isAssignableFrom(method.getParameterTypes()[0])) {
            return (Class<? extends Entity>) method.getParameterTypes()[0];
        }

        return null;
    }
    @SuppressWarnings("unchecked")
    static Class<? extends Entity> getEntityTypeIfMethodReturnsEntityOrCollectionOfEntities(Method method) {
        Class returnType = method.getReturnType();
        Type genericReturnType = method.getGenericReturnType();

        if (Entity.class.isAssignableFrom(returnType)) {
            return returnType;
        } else if (genericReturnType instanceof ParameterizedType) {
            Type type = ((ParameterizedType) genericReturnType).getActualTypeArguments()[0];
            if (type instanceof Class && Entity.class.isAssignableFrom((Class) type)) {
                return (Class) type;
            }
        }

        return null;
    }
}
