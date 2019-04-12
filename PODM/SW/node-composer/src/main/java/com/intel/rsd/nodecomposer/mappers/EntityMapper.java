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
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;

import static com.intel.rsd.nodecomposer.mappers.Conditions.refMappingCondition;

public abstract class EntityMapper<S extends ExternalServiceResource, T extends DiscoverableEntity> extends BrilliantMapper<S, T> {
    protected S source;
    protected T target;

    protected EntityMapper(Class<S> sourceClass, Class<T> targetClass) {
        super(sourceClass, targetClass);
        setMappingConditions(refMappingCondition());
    }

    public boolean canMap(ExternalServiceResource source) {
        return getSourceClass().isAssignableFrom(source.getClass());
    }

    @Override
    public void map(S source, T target) {
        this.source = source;
        this.target = target;
        super.map(source, target);
    }
}
