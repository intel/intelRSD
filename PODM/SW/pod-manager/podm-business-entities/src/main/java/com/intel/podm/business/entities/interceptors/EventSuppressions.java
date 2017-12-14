/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.Entity;

import javax.enterprise.context.ApplicationScoped;
import java.util.List;

import static com.google.common.cache.CacheBuilder.newBuilder;
import static com.intel.podm.business.entities.interceptors.EventableAnnotationsUtils.isEventable;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.reflect.FieldUtils.getFieldsListWithAnnotation;

@ApplicationScoped
public class EventSuppressions {
    private static final int CACHE_SIZE = 1000;

    private final LoadingCache<Class<?>, List<String>> fieldLevelSuppressionsByClass =
        newBuilder()
            .maximumSize(CACHE_SIZE)
            .build(
                new CacheLoader<Class<?>, List<String>>() {
                    @Override
                    public List<String> load(Class<?> key) throws Exception {
                        return suppressedFields(key);
                    }
                });

    private final LoadingCache<Class<?>, Boolean> classLevelSuppressionsByClass =
        newBuilder()
            .maximumSize(CACHE_SIZE)
            .build(
                new CacheLoader<Class<?>, Boolean>() {
                    @Override
                    public Boolean load(Class<?> key) throws Exception {
                        return isEventable(key);
                    }
                });


    public boolean isSuppressed(Entity entity, String fieldName) {
        return isSuppressed(entity) || fieldLevelSuppressionsByClass.getUnchecked(entity.getClass()).contains(fieldName);
    }

    public boolean isSuppressed(Entity entity) {
        return !classLevelSuppressionsByClass.getUnchecked(entity.getClass());
    }

    private List<String> suppressedFields(Class<?> clazz) {
        return getFieldsListWithAnnotation(clazz, SuppressEvents.class).stream()
            .map(field -> format("%s.%s", clazz.getName(), field.getName()))
            .collect(toList());
    }
}
