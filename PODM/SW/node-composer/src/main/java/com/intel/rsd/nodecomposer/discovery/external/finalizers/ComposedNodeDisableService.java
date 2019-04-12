/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLED;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ComposedNodeDisableService {
    @Transactional(MANDATORY)
    public void disableComposedNode(ComposedNode composedNode) {
        if (composedNode.isInAnyOfStates(ASSEMBLED)) {
            composedNode.setEligibleForRecovery(true);
        }

        composedNode.setComposedNodeState(FAILED);
        composedNode.setStatus(OFFLINE_CRITICAL_STATUS);
        log.info(format("Putting composed Node: %s into %s state and %s status",
            composedNode.getComposedNodeId(),
            FAILED,
            OFFLINE_CRITICAL_STATUS
        ));
    }

    @Transactional(MANDATORY)
    public void disableFromAssets(Set<ComposableAsset> degradedAssets) {
        degradedAssets.stream()
            .peek(asset -> log.trace("Degraded asset: " + asset))
            .map(ComposableAsset::getComposedNode)
            .filter(Objects::nonNull)
            .forEach(this::disableComposedNode);
    }
}
