/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.entities.redfish.components;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.components.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class ComposedNodeUpdater {
    @Inject
    private Logger logger;

    @Transactional(REQUIRED)
    public void updateRelatedComposedNodes(Set<DomainObject> domainObjects) {
        List<ComputerSystem> computerSystems = getNotEnabledAssetsOfType(domainObjects, ComputerSystem.class);
        computerSystems.stream()
                .map(ComputerSystem::getComposedNode)
                .filter(Objects::nonNull)
                .forEach(composedNode -> disableComposedNode(composedNode, "Not enabled Computer System"));

        List<RemoteTarget> targets = getNotEnabledAssetsOfType(domainObjects, RemoteTarget.class);
        targets.stream()
                .map(RemoteTarget::getComposedNode)
                .filter(Objects::nonNull)
                .forEach(composedNode -> disableComposedNode(composedNode, "Not enabled Remote Target"));
    }

    private <T extends StatusPossessor> List<T> getNotEnabledAssetsOfType(Set<DomainObject> domainObjects, Class<T> clazz) {
        return domainObjects.stream()
                .filter(Objects::nonNull)
                .filter(domainObject -> clazz.isAssignableFrom(domainObject.getClass()))
                .map(clazz::cast)
                .filter(statusPossessor -> !statusPossessor.isEnabledAndHealthy())
                .peek(notEnabledDomainObject -> logger.d("Not enabled domain object: " + notEnabledDomainObject))
                .collect(toList());
    }

    @Transactional(MANDATORY)
    public void disableComposedNode(ComposedNode composedNode, String reason) {
        composedNode.setStatus(OFFLINE_CRITICAL_STATUS);
        composedNode.setComposedNodeState(FAILED);

        String msg = "Failing Composed Node [" + composedNode.getId() + "]";
        if (reason != null) {
            msg += " because of: " + reason;
        }

        logger.d(msg);
    }
}
