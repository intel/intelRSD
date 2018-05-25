/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.MetricDefinitionWildcard;
import com.intel.podm.client.resources.redfish.WildcardObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class WildcardMapper extends SubresourceMapper<WildcardObject, MetricDefinitionWildcard> {

    @Inject
    SimpleTypeMapper simpleTypeMapper;

    @Inject
    public WildcardMapper(EmbeddableCleanAndCreateStrategy<MetricDefinitionWildcard> embeddableCleanAndCreateStrategy) {
        super(MetricDefinitionWildcard.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(WildcardObject source, MetricDefinitionWildcard target) {
        target.setName(source.getName());
        simpleTypeMapper.map(
            source.getValues(),
            target.getValues(),
            target::addValues
        );
    }

    @Override
    protected boolean equals(WildcardObject source, MetricDefinitionWildcard target) {
        return Objects.equals(source.getName(), target.getName())
            && Objects.deepEquals(source.getValues(), target.getValues());

    }
}
