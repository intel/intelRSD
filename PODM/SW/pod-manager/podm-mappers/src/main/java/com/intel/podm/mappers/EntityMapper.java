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

package com.intel.podm.mappers;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.mappers.subresources.UnknownOemMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import static com.intel.podm.mappers.Conditions.refMappingCondition;

public abstract class EntityMapper<S extends ExternalServiceResource, T extends DiscoverableEntity> extends BrilliantMapper<S, T> {
    protected S source;
    protected T target;

    private UnknownOemMapper unknownOemMapper = new UnknownOemMapper(new EmbeddableCleanAndCreateStrategy<>());

    protected EntityMapper(Class<S> sourceClass, Class<T> targetClass) {
        super(sourceClass, targetClass);
        setMappingConditions(refMappingCondition());
    }

    public boolean canMap(ExternalServiceResource source) {
        return getSourceClass().isAssignableFrom(source.getClass());
    }

    @Override
    protected void performNotAutomatedMapping(S source, T target) {
        super.performNotAutomatedMapping(source, target);
        unknownOemMapper.map(source.getUnknownOems(), target.getUnknownOems(), target::addUnknownOem);
    }

    @Override
    public void map(S source, T target) {
        this.source = source;
        this.target = target;
        super.map(source, target);
    }
}
