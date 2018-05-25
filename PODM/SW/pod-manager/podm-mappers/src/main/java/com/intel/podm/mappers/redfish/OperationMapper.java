/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.embeddables.Operation;
import com.intel.podm.client.resources.redfish.OperationResource;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class OperationMapper extends SubresourceMapper<OperationResource, Operation> {
    @Inject
    public OperationMapper(EmbeddableCleanAndCreateStrategy<Operation> embeddableCleanAndCreateStrategy) {
        super(Operation.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(OperationResource operationsResource, Operation operations) {
        operations.setOperationName(operationsResource.getOperationName());
        operations.setPercentageComplete(operationsResource.getPercentageComplete());
    }

    @Override
    protected boolean equals(OperationResource operationsResource, Operation operations) {
        return Objects.equals(operationsResource.getOperationName(), operations.getOperationName())
            && Objects.equals(operationsResource.getPercentageComplete(), operations.getPercentageComplete());
    }
}
