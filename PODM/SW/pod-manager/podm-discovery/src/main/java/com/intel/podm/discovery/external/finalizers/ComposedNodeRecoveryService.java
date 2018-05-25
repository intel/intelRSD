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

import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.base.ComposableAsset;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.utils.Collections.filterByType;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ComposedNodeRecoveryService {
    @Inject
    private Logger logger;

    @Inject
    private ComposedNodeDao composedNodeDao;

    @Inject
    private Recoupler<Drive> driveRecoupler;

    @Inject
    private Recoupler<Endpoint> endpointRecoupler;

    @Inject
    private Recoupler<Volume> volumeRecoupler;

    @Inject
    private Recoupler<ComputerSystem> computerSystemRecoupler;

    @Transactional(MANDATORY)
    void recoverFromAssets(Set<ComposableAsset> assetsAvailableToAttach) {
        Collection<Drive> drives = filterByType(assetsAvailableToAttach, Drive.class);
        Collection<ComputerSystem> systems = filterByType(assetsAvailableToAttach, ComputerSystem.class);
        Collection<Endpoint> endpoints = filterByType(assetsAvailableToAttach, Endpoint.class);
        Collection<Volume> volumes = filterByType(assetsAvailableToAttach, Volume.class);

        for (ComposedNode node : composedNodeDao.getComposedNodesEligibleForRecovery()) {
            if (driveRecoupler.recouple(node, drives)
                && computerSystemRecoupler.recouple(node, systems)
                && endpointRecoupler.recouple(node, endpoints)
                && volumeRecoupler.recouple(node, volumes)) {
                recoverComposedNode(node);
            }
        }
    }

    private void recoverComposedNode(ComposedNode node) {
        ComposedNodeState composedNodeState = ASSEMBLED;
        Status composedNodeStatus = new Status(ENABLED, OK, OK);

        node.setEligibleForRecovery(false);
        node.setStatus(composedNodeStatus);

        node.setComposedNodeState(composedNodeState);
        logger.i(format("Putting composed Node: %s into %s state and %s status",
            node.getId(),
            composedNodeState,
            composedNodeStatus));
    }
}
