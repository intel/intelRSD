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

import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLED;
import static com.intel.rsd.nodecomposer.types.Health.OK;
import static com.intel.rsd.nodecomposer.types.State.ENABLED;
import static com.intel.rsd.nodecomposer.utils.Collections.filterByType;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ComposedNodeRecoveryService {
    private final ComposedNodeDao composedNodeDao;
    private final Recoupler<Drive> driveRecoupler;
    private final Recoupler<Endpoint> endpointRecoupler;
    private final Recoupler<Volume> volumeRecoupler;
    private final Recoupler<ComputerSystem> computerSystemRecoupler;
    private final Recoupler<Processor> processorsRecoupler;

    @Autowired
    @SuppressWarnings({"checkstyle:ParameterNumber"})
    public ComposedNodeRecoveryService(ComposedNodeDao composedNodeDao, Recoupler<Drive> driveRecoupler, Recoupler<Endpoint> endpointRecoupler,
                                       Recoupler<Volume> volumeRecoupler, Recoupler<ComputerSystem> computerSystemRecoupler,
                                       Recoupler<Processor> processorsRecoupler) {
        this.composedNodeDao = composedNodeDao;
        this.driveRecoupler = driveRecoupler;
        this.endpointRecoupler = endpointRecoupler;
        this.volumeRecoupler = volumeRecoupler;
        this.computerSystemRecoupler = computerSystemRecoupler;
        this.processorsRecoupler = processorsRecoupler;
    }

    @Transactional(MANDATORY)
    @SuppressWarnings({"checkstyle:CyclomaticComplexity", "checkstyle:BooleanExpressionComplexity"})
    void recoverFromAssets(Set<ComposableAsset> assetsAvailableToAttach) {
        Collection<Drive> drives = filterByType(assetsAvailableToAttach, Drive.class);
        Collection<ComputerSystem> systems = filterByType(assetsAvailableToAttach, ComputerSystem.class);
        Collection<Endpoint> endpoints = filterByType(assetsAvailableToAttach, Endpoint.class);
        Collection<Volume> volumes = filterByType(assetsAvailableToAttach, Volume.class);
        Collection<Processor> processors = filterByType(assetsAvailableToAttach, Processor.class);

        for (ComposedNode node : composedNodeDao.getComposedNodesEligibleForRecovery()) {
            boolean shouldNodeBeRecovered = driveRecoupler.recouple(node, drives)
                && computerSystemRecoupler.recouple(node, systems)
                && endpointRecoupler.recouple(node, endpoints)
                && volumeRecoupler.recouple(node, volumes)
                && processorsRecoupler.recouple(node, processors);
            if (shouldNodeBeRecovered) {
                recoverComposedNode(node);
            }
        }
    }

    private void recoverComposedNode(ComposedNode node) {
        node.setEligibleForRecovery(false);
        node.setStatus(new Status(ENABLED, OK, OK));
        node.setComposedNodeState(ASSEMBLED);
        log.info(format("Putting composed Node: %s into %s state and %s status",
            node.getComposedNodeId(),
            node.getComposedNodeState(),
            node.getStatus()));
    }
}
