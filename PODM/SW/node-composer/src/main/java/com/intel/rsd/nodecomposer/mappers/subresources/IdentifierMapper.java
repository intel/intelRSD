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

package com.intel.rsd.nodecomposer.mappers.subresources;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.IdentifierObject;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class IdentifierMapper extends SubresourceMapper<IdentifierObject, Identifier> {
    @Autowired
    public IdentifierMapper(EmbeddableCleanAndCreateStrategy<Identifier> embeddableCleanAndCreateStrategy) {
        super(Identifier.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(IdentifierObject sourceIdentifier, Identifier targetIdentifier) {
        if (sourceIdentifier != null) {
            targetIdentifier.setDurableName(sourceIdentifier.getDurableName());
            targetIdentifier.setDurableNameFormat(sourceIdentifier.getDurableNameFormat());
        }
    }

    @Override
    public boolean equals(IdentifierObject sourceIdentifier, Identifier targetIdentifier) {
        return Objects.equals(sourceIdentifier.getDurableName(), targetIdentifier.getDurableName())
            && Objects.equals(sourceIdentifier.getDurableNameFormat(), targetIdentifier.getDurableNameFormat());
    }
}
