/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers;

import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Optional;
import java.util.Set;

import static java.util.Optional.empty;
import static java.util.Optional.of;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class MapperFinder {
    private final NodeComposerBeanFactory beanFactory;
    private Set<EntityMapper> cache;

    @Autowired
    public MapperFinder(NodeComposerBeanFactory beanFactory) {
        this.beanFactory = beanFactory;
    }

    public Optional<Mapper<ExternalServiceResource, Entity>> find(ExternalServiceResource resource) {
        for (EntityMapper mapper : getMapperPool()) {
            if (mapper.canMap(resource)) {
                return of(mapper);
            }
        }

        return empty();
    }

    private Set<EntityMapper> getMapperPool() {
        if (cache == null) {
            cache = beanFactory.getBeansOfType(EntityMapper.class);
        }
        return cache;
    }
}
