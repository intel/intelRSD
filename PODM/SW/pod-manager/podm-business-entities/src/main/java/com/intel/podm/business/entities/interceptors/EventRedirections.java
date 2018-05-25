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
import com.google.common.cache.LoadingCache;
import com.intel.podm.business.entities.EventRedirectionSource;
import com.intel.podm.business.entities.EventRedirectionTarget;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;
import com.intel.podm.common.types.Pair;

import javax.enterprise.context.ApplicationScoped;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Predicate;
import java.util.stream.Stream;

import static com.google.common.cache.CacheBuilder.newBuilder;
import static com.intel.podm.common.types.Pair.pairOf;
import static org.apache.commons.lang3.reflect.FieldUtils.getFieldsListWithAnnotation;
import static org.apache.commons.lang3.reflect.MethodUtils.getMethodsListWithAnnotation;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;

@ApplicationScoped
public class EventRedirections {

    private static final int CACHE_SIZE = 1000;

    private final LoadingCache<Class<?>, List<Redirection>> redirectionsBySourceClass =
        newBuilder()
            .maximumSize(CACHE_SIZE)
            .build(
                new CacheLoader<Class<?>, List<Redirection>>() {
                    @Override
                    public List<Redirection> load(Class<?> key) throws Exception {
                        return initializeRedirections(key);
                    }
                });


    public void redirectIfRequired(Entity sourceEntity, Predicate<String> redirectionRequired, Consumer<Entity> redirector) {
        redirectionsBySourceClass.getUnchecked(sourceEntity.getClass())
            .stream()
            .filter(redirection -> redirectionRequired.test(redirection.getSourceFieldName()))
            .map(redirection -> redirection.findTargetEntity(sourceEntity))
            .forEach(optionalTarget -> optionalTarget.ifPresent(redirector));
    }

    private List<Redirection> initializeRedirections(Class<?> clazz) {
        Map<String, Method> targetsByKey = findTargets(clazz);
        Map<String, List<Field>> sourcesByKey = findSources(clazz);

        return sourcesByKey.entrySet().stream()
            .filter(entry -> targetsByKey.containsKey(entry.getKey()))
            .flatMap(entry -> mapToRedirections(entry.getValue(), targetsByKey.get(entry.getKey())))
            .collect(toList());
    }

    private Map<String, List<Field>> findSources(Class<?> clazz) {
        return getFieldsListWithAnnotation(clazz, EventRedirectionSource.class)
            .stream()
            .map(redirectionSource -> pairOf(findRedirectionKey(redirectionSource), redirectionSource))
            .collect(groupingBy(pair -> pair.first(), mapping(Pair::second, toList())));
    }

    private Map<String, Method> findTargets(Class<?> clazz) {
        return getMethodsListWithAnnotation(clazz, EventRedirectionTarget.class)
            .stream()
            .map(redirectionTarget -> pairOf(findRedirectionKey(redirectionTarget), redirectionTarget))
            .collect(toMap(Pair::first, Pair::second));
    }

    private Stream<Redirection> mapToRedirections(Collection<Field> redirectionSources, Method redirectionTargetProvider) {
        return redirectionSources.stream().map(source -> new Redirection(source.getName(), redirectionTargetProvider));
    }

    private String findRedirectionKey(Method target) {
        return target.getAnnotation(EventRedirectionTarget.class).value();
    }

    private String findRedirectionKey(Field redirectionSource) {
        return redirectionSource.getAnnotation(EventRedirectionSource.class).value();
    }

    public static class Redirection {

        private static final Logger LOGGER = LoggerFactory.getLogger(Redirection.class);

        private final Method targetProvider;

        private String sourceFieldName;

        public Redirection(String sourceFieldName, Method targetProvider) {
            this.sourceFieldName = sourceFieldName;
            this.targetProvider = targetProvider;
        }

        public String getSourceFieldName() {
            return sourceFieldName;
        }

        public Optional<Entity> findTargetEntity(Entity sourceEntity) {
            try {
                return ofNullable((Entity) targetProvider.invoke(sourceEntity));
            } catch (IllegalAccessException | InvocationTargetException e) {
                LOGGER.e("Redirection target cannot be found for entity({})", sourceEntity);
                return empty();
            }
        }
    }
}
