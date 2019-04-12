/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;

import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.ClassGatherer.getAllClassesByPackageAndAnnotation;
import static java.util.stream.Collectors.toSet;

@SuppressWarnings({"checkstyle:MethodLength"})
final class EntityDetailsGatherer {
    private static final String ENTITIES_PACKAGE = "com.intel.rsd.nodecomposer.persistence.*";
    private static final Set<EntityDetail> ENTITY_DETAILS = createEntityDetails(getAllEntities());
    private static final Set<Object> EMBEDDABLES = createEmbeddables(getAllEmbeddables());

    private EntityDetailsGatherer() {
    }

    static Set<EntityDetail> getEntityDetails() {
        return ENTITY_DETAILS;
    }

    static Set<Object> getEmbeddables() {
        return EMBEDDABLES;
    }

    private static Set<EntityDetail> createEntityDetails(Set<Class<? extends Entity>> classes) {
        return classes.stream()
            .map(EntityDetailsGatherer::createEntityDetail)
            .collect(toSet());
    }

    private static Set<Object> createEmbeddables(Set<Class<?>> classes) {
        return classes.stream()
            .map(EntityDetailsGatherer::instantiateEmbeddable)
            .collect(toSet());
    }

    private static EntityDetail createEntityDetail(Class<? extends Entity> clazz) {
        return new EntityDetail(clazz);
    }

    private static Object instantiateEmbeddable(Class<?> clazz) {
        try {
            return clazz.newInstance();
        } catch (IllegalAccessException | InstantiationException e) {
            throw new RuntimeException(e);
        }
    }

    @SuppressWarnings("unchecked")
    private static Set<Class<? extends Entity>> getAllEntities() {
        return getAllClassesByPackageAndAnnotation(ENTITIES_PACKAGE, javax.persistence.Entity.class).stream()
            .map(clazz -> (Class<? extends Entity>) clazz)
            .collect(toSet());
    }

    private static Set<Class<?>> getAllEmbeddables() {
        return getAllClassesByPackageAndAnnotation(ENTITIES_PACKAGE, javax.persistence.Embeddable.class);
    }
}
