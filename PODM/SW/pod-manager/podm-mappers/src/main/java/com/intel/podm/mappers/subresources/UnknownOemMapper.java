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

import com.intel.podm.business.entities.redfish.embeddables.UnknownOem;
import com.intel.podm.client.resources.redfish.UnknownOemObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import java.util.Objects;

public class UnknownOemMapper extends SubresourceMapper<UnknownOemObject, UnknownOem> {
    public UnknownOemMapper(EmbeddableCleanAndCreateStrategy<UnknownOem> embeddableCleanAndCreateStrategy) {
        super(UnknownOem.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(UnknownOemObject unknownOemObject, UnknownOem unknownOem) {
        unknownOem.setOemPath(unknownOemObject.getOemPath());
        unknownOem.setOemValue(unknownOemObject.getOemValue());
    }

    @Override
    protected boolean equals(UnknownOemObject unknownOemObject, UnknownOem unknownOem) {
        return Objects.equals(unknownOemObject.getOemPath(), unknownOem.getOemPath())
            && Objects.equals(unknownOemObject.getOemValue(), unknownOem.getOemValue());
    }
}
