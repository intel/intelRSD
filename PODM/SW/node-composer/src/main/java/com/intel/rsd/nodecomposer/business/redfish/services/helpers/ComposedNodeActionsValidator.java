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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.ComposedNodeAssetNotAvailableException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;

@Component
public class ComposedNodeActionsValidator {
    private static final int RETRY_AFTER_SECONDS = 30;

    public void validateIfActionCanBePerformedOnNode(ComposedNode composedNode) throws ResourceStateMismatchException, ComposedNodeAssetNotAvailableException {
        if (composedNode.getComputerSystem() == null) {
            throw new ComposedNodeAssetNotAvailableException("There is no Computer System related with requested Node.", RETRY_AFTER_SECONDS);
        }
        if (!composedNode.isInAnyOfStates(ASSEMBLED, FAILED)) {
            throw new ResourceStateMismatchException("Composed Node should be assembled in order to invoke this action on it.");
        }
    }
}
