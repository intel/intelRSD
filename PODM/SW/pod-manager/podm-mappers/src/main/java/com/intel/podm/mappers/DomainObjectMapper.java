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

import java.util.Objects;

import static com.intel.podm.mappers.MappingMethod.BASIC;

public abstract class DomainObjectMapper<S extends ExternalServiceResource, T extends DomainObject> extends BrilliantMapper<S, T> {
    protected S source;
    protected T target;
    private final MappingMethod supportedMappingMethod;

    protected DomainObjectMapper(Class<S> sourceClass, Class<T> targetClass) {
        this(sourceClass, targetClass, BASIC);
    }

    protected DomainObjectMapper(Class<S> sourceClass, Class<T> targetClass, MappingMethod supportedMappingMethod) {
        super(sourceClass, targetClass);
        this.supportedMappingMethod = supportedMappingMethod;
    }

    public boolean canMap(ExternalServiceResource source, MappingMethod mappingMethod) {
        return getSourceClass().isAssignableFrom(source.getClass()) && Objects.equals(supportedMappingMethod, mappingMethod);
    }

    @Override
    public void map(S source, T target) {
        this.source = source;
        this.target = target;
        super.map(source, target);
    }
}
