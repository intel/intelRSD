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

import com.intel.podm.business.entities.redfish.embeddables.RedfishLocation;
import com.intel.podm.client.api.resources.redfish.RedfishLocationObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class RedfishLocationMapper extends SubresourceMapper<RedfishLocationObject, RedfishLocation> {
    @Inject
    public RedfishLocationMapper(EmbeddableCleanAndCreateStrategy<RedfishLocation> embeddableCleanAndCreateStrategy) {
        super(RedfishLocation.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(RedfishLocationObject sourceLocation, RedfishLocation targetLocation) {
        targetLocation.setInfo(sourceLocation.getInfo());
        targetLocation.setInfoFormat(sourceLocation.getInfoFormat());
    }

    @Override
    public boolean equals(RedfishLocationObject sourceLocation, RedfishLocation targetLocation) {
        return Objects.equals(sourceLocation.getInfo(), targetLocation.getInfo())
                && Objects.equals(sourceLocation.getInfoFormat(), targetLocation.getInfoFormat());
    }
}
