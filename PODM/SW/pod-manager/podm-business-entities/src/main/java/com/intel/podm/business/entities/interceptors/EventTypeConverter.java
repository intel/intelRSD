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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.events.EventType;

import javax.enterprise.context.ApplicationScoped;
import java.util.function.BiFunction;
import java.util.function.Function;

import static com.google.common.cache.CacheBuilder.newBuilder;
import static com.intel.podm.business.entities.interceptors.EventableAnnotationsUtils.hasEventOriginProvider;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;
import static java.util.function.Function.identity;

@ApplicationScoped
public class EventTypeConverter implements BiFunction<Entity, EventType, EventType> {
    private static final int CACHE_SIZE = 1000;

    private static final Function<EventType, EventType> ANY_AS_UPDATE_STATE_AND_ALERT_UNCHANGED = eventType -> {
        switch (eventType) {
            case ALERT:
            case STATUS_CHANGE:
                return eventType;
            default:
                return RESOURCE_UPDATED;
        }
    };

    private LoadingCache<Class<?>, Function<EventType, EventType>> eventConvertersByClass
        = newBuilder()
        .maximumSize(CACHE_SIZE)
        .build(new CacheLoader<Class<?>, Function<EventType, EventType>>() {
            @Override
            public Function<EventType, EventType> load(Class<?> key) throws Exception {
                if (hasEventOriginProvider(key)) {
                    return ANY_AS_UPDATE_STATE_AND_ALERT_UNCHANGED;
                }
                return identity();
            }
        });

    @Override
    public EventType apply(Entity sourceEntity, EventType sourceEventType) {
        EventType eventType;
        if (isComplementaryEntity(sourceEntity)) {
            eventType = RESOURCE_UPDATED;
        } else {
            eventType = eventConvertersByClass.getUnchecked(sourceEntity.getClass()).apply(sourceEventType);
        }

        return eventType;
    }

    private boolean isComplementaryEntity(Entity entity) {
        if (entity instanceof DiscoverableEntity) {
            return ((DiscoverableEntity) entity).isComplementary();
        }
        return false;
    }
}
