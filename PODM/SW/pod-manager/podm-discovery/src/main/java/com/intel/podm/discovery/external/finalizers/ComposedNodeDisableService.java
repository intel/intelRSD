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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.base.ComposableAsset;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ComposedNodeDisableService {
    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void disableComposedNode(ComposedNode composedNode) {
        if (composedNode.isInAnyOfStates(ASSEMBLED)) {
            composedNode.setEligibleForRecovery(true);
        }

        composedNode.setComposedNodeState(FAILED);
        composedNode.setStatus(OFFLINE_CRITICAL_STATUS);
        logger.i(format("Putting composed Node: %s into %s state and %s status",
            composedNode.getId(),
            FAILED,
            OFFLINE_CRITICAL_STATUS
        ));
    }

    @Transactional(MANDATORY)
    public void disableFromAssets(Set<ComposableAsset> degradedAssets) {
        degradedAssets.stream()
            .peek(asset -> logger.t("Degraded asset: " + asset))
            .map(ComposableAsset::getComposedNode)
            .filter(Objects::nonNull)
            .forEach(this::disableComposedNode);
    }
}
