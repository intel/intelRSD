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

package com.intel.podm.mappers;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.client.api.resources.ExternalServiceResource;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Optional;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.mappers.MappingMethod.BASIC;
import static com.intel.podm.mappers.MappingMethod.DEEP;
import static com.intel.podm.mappers.MappingMethod.FULL;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@Dependent
public class MapperFinder {
    @Inject
    private Instance<DomainObjectMapper<? extends ExternalServiceResource, ? extends DomainObject>> mapperPool;

    private Collection<DomainObjectMapper<? extends ExternalServiceResource, ? extends DomainObject>> cache;

    public Optional<Mapper<ExternalServiceResource, DomainObject>> find(ExternalServiceResource resource, MappingMethod method) {
        if (method == FULL) {
            return composeFullMapper(resource);
        }

        for (DomainObjectMapper mapper : getMapperPool()) {
            if (mapper.canMap(resource, method)) {
                return of(mapper);
            }
        }

        return empty();
    }

    private Iterable<DomainObjectMapper<? extends ExternalServiceResource, ? extends DomainObject>> getMapperPool() {
        if (cache == null) {
            cache = newArrayList(mapperPool);
        }

        return cache;
    }

    private Optional<Mapper<ExternalServiceResource, DomainObject>> composeFullMapper(ExternalServiceResource resource) {
        Optional<Mapper<ExternalServiceResource, DomainObject>> psmeMapper = find(resource, BASIC);
        Optional<Mapper<ExternalServiceResource, DomainObject>> luiMapper = find(resource, DEEP);

        return luiMapper.isPresent()
                ? psmeMapper.map(pm -> new ComposedMapper(pm, luiMapper.get()))
                : psmeMapper;
    }

    private static final class ComposedMapper implements Mapper<ExternalServiceResource, DomainObject> {
        private final Mapper<ExternalServiceResource, DomainObject> psmeMapper;
        private final Mapper<ExternalServiceResource, DomainObject> luiMapper;

        ComposedMapper(Mapper<ExternalServiceResource, DomainObject> psmeMapper, Mapper<ExternalServiceResource, DomainObject> luiMapper) {
            this.psmeMapper = psmeMapper;
            this.luiMapper = luiMapper;
        }

        @Override
        public void map(ExternalServiceResource source, DomainObject target) {
            psmeMapper.map(source, target);
            luiMapper.map(source, target);
        }

        @Override
        public Class<? extends ExternalServiceResource> getSourceClass() {
            return psmeMapper.getSourceClass();
        }

        @Override
        public Class<? extends DomainObject> getTargetClass() {
            return psmeMapper.getTargetClass();
        }
    }
}
