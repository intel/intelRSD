/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.interceptors;

import com.google.common.cache.CacheLoader;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.resolvers.MultiSourceEntityResolver;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Function;

import static com.intel.podm.business.entities.interceptors.EventableAnnotationsUtils.getEventOriginProvider;
import static java.util.function.Function.identity;

public class EventOriginInfoProviderCacheLoader extends CacheLoader<Class<?>, Function<Entity, Entity>> {
    private List<MultiSourceEntityResolver> multiSourceEntityResolvers;

    public EventOriginInfoProviderCacheLoader(List<MultiSourceEntityResolver> multiSourceEntityResolvers) {
        this.multiSourceEntityResolvers = multiSourceEntityResolvers;
    }

    @Override
    public Function<Entity, Entity> load(Class<?> key) throws Exception {
        Function<Entity, Entity> eventOriginProvider = getEventOriginProvider(key).map(this::toFunction).orElse(identity());
        if (isMultiSource(key)) {
            eventOriginProvider = eventOriginProvider.andThen(createMultiSourceEntityConverter());
        }
        return eventOriginProvider;
    }

    private Function<Entity, Entity> toFunction(Method method) {
        return new MethodInvoker(method);
    }

    private boolean isMultiSource(Class<?> clazz) {
        return MultiSourceResource.class.isAssignableFrom(clazz);
    }

    private Function<Entity, Entity> createMultiSourceEntityConverter() {
        return entity -> findMultiSourceResolver(entity.getClass()).apply(entity);
    }

    private Function<Entity, Entity> findMultiSourceResolver(Class<? extends Entity> clazz) {
        Optional<MultiSourceEntityResolver> sourceResolver = multiSourceEntityResolvers.stream()
            .filter(multiSourceEntityResolver -> Objects.equals(multiSourceEntityResolver.getEntityClass(), clazz))
            .findFirst();

        return sourceResolver.isPresent() ? sourceResolver.get() : identity();
    }

    static class MethodInvoker implements Function<Entity, Entity> {
        private Method method;

        MethodInvoker(Method method) {
            this.method = method;
        }

        @Override
        public Entity apply(Entity o) {
            try {
                return (Entity) method.invoke(o);
            } catch (IllegalAccessException | InvocationTargetException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
