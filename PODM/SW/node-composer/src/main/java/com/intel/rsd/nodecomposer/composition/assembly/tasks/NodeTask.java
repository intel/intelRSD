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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasuredClassIdentityProvider;
import com.intel.rsd.nodecomposer.utils.throwing.ThrowingRunnable;
import lombok.Setter;

import java.io.Serializable;

import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static org.apache.commons.lang3.StringUtils.substringBefore;

/**
 * Child classes are obligated to rethrow RuntimeException on exception to ensure transaction will be rolled back
 */
@TimeMeasuredClassIdentityProvider(NodeTaskClassIdentityProvider.class)
public abstract class NodeTask implements ThrowingRunnable<RuntimeException>, Serializable {
    private static final long serialVersionUID = 2348082215373182739L;

    @Setter
    protected ODataId composedNodeODataId;

    ComputerSystem getComputerSystemFromNode(ComposedNode node) {
        return ofNullable(node.getComputerSystem()).orElseThrow(() ->
            new RuntimeException(format("Unknown Computer System for Node: %s", node.getUri())));
    }

    @Override
    public String toString() {
        return format("%s (%s)", substringBefore(getClass().getSimpleName(), "$"), composedNodeODataId);
    }
}
