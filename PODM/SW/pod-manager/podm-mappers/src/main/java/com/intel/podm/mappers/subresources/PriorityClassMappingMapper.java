/*
 * Copyright (c) 2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.embeddables.PriorityClassMapping;
import com.intel.podm.client.resources.redfish.DcbxConfigObject.PriorityClassMappingObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class PriorityClassMappingMapper extends SubresourceMapper<PriorityClassMappingObject, PriorityClassMapping> {

    @Inject
    public PriorityClassMappingMapper(EmbeddableCleanAndCreateStrategy<PriorityClassMapping> entityCleanAndCreateStrategy) {
        super(PriorityClassMapping.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(PriorityClassMappingObject source, PriorityClassMapping target) {
        target.setPriorityGroup(source.getPriorityGroup());
        target.setPriority(source.getPriority());
    }

    @Override
    protected boolean equals(PriorityClassMappingObject source, PriorityClassMapping target) {
        return Objects.equals(source.getPriority(), target.getPriority())
            && Objects.equals(source.getPriorityGroup(), target.getPriorityGroup());
    }
}
