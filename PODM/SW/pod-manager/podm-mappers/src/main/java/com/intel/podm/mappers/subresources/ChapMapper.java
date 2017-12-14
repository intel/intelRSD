/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.embeddables.Chap;
import com.intel.podm.client.resources.redfish.ChapObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class ChapMapper extends SubresourceMapper<ChapObject, Chap> {

    @Inject
    public ChapMapper(EmbeddableCleanAndCreateStrategy<Chap> embeddableCleanAndCreateStrategy) {
        super(Chap.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(ChapObject sourceChap, Chap targetChap) {
        targetChap.setType(sourceChap.getType());
        targetChap.setUsername(sourceChap.getUsername());
        targetChap.setMutualUsername(sourceChap.getMutualUsername());
    }

    @Override
    protected boolean equals(ChapObject sourceChap, Chap targetChap) {
        return new EqualsBuilder()
            .append(sourceChap.getType(), targetChap.getType())
            .append(sourceChap.getUsername(), targetChap.getUsername())
            .append(sourceChap.getMutualUsername(), targetChap.getMutualUsername())
            .isEquals();
    }
}
