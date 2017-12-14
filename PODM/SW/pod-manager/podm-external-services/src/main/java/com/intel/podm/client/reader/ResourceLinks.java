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

package com.intel.podm.client.reader;

import com.google.common.reflect.TypeToken;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.ExternalServiceResource;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.Collection;
import java.util.HashMap;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Collections.emptyList;
import static java.util.Collections.singleton;

public class ResourceLinks {
    private static final TypeToken SUPPLIER_TYPE_TOKEN = new TypeToken<ResourceSupplier>() {
    };

    private final HashMap<String, Method> methods = new HashMap<>();
    private final ExternalServiceResource resource;

    public ResourceLinks(ExternalServiceResource resource) {
        this.resource = resource;

        for (Method method : resource.getClass().getMethods()) {
            if (hasAcceptableReturnType(method) && hasLinkNameAnnotation(method)) {
                String name = method.getAnnotation(LinkName.class).value();
                methods.put(name, method);
            }
        }
    }

    public Collection<String> getNames() {
        return methods.keySet();
    }

    @SuppressWarnings({"unchecked"})
    public Iterable<ResourceSupplier> get(String name) throws WebClientRequestException {
        requiresNonNull(name, "name");

        if (!methods.containsKey(name)) {
            throw new IllegalArgumentException("No method found for given name");
        }

        Method method = methods.get(name);
        Object result = invoke(method);
        if (result == null) {
            return emptyList();
        }

        return isSupplierType(method.getGenericReturnType())
            ? singleton((ResourceSupplier) result)
            : (Iterable<ResourceSupplier>) result;
    }

    private Object invoke(Method method) throws WebClientRequestException {
        Object result;
        try {
            result = method.invoke(resource);
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e);
        } catch (InvocationTargetException e) {
            if (e.getTargetException() instanceof WebClientRequestException) {
                throw (WebClientRequestException) e.getTargetException();
            }

            throw new RuntimeException(e);
        }
        return result;
    }

    private static boolean hasLinkNameAnnotation(Method method) {
        return method.isAnnotationPresent(LinkName.class);
    }

    private static boolean hasAcceptableReturnType(Method method) {
        if (!Iterable.class.isAssignableFrom(method.getReturnType())) {
            return isSupplierType(method.getGenericReturnType());
        }

        ParameterizedType parameterizedType = (ParameterizedType) TypeToken.of(method.getGenericReturnType()).getType();
        Type iterableParamType = parameterizedType.getActualTypeArguments()[0];

        return SUPPLIER_TYPE_TOKEN.isAssignableFrom(iterableParamType);
    }

    private static boolean isSupplierType(Type type) {
        return SUPPLIER_TYPE_TOKEN.isAssignableFrom(type);
    }
}
