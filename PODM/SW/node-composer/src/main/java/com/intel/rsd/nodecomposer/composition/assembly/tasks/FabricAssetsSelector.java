/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Objects;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.Stream.concat;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class FabricAssetsSelector {
    @Transactional(MANDATORY)
    Collection<DiscoverableEntity> selectRelatedAssets(ComposedNode node) {
        Collection<Endpoint> allEndpoints = selectEndpoints(node);
        Collection<DiscoverableEntity> allZones = selectZones(allEndpoints);
        Collection<DiscoverableEntity> allVolumes = selectVolumes(node, allEndpoints);
        Collection<DiscoverableEntity> allFpgaProcessors = selectFpgaProcessors(node);
        ArrayList<DiscoverableEntity> result = new ArrayList<>();
        result.addAll(allZones);
        result.addAll(allEndpoints);
        result.addAll(allVolumes);
        result.addAll(allFpgaProcessors);
        log.debug("Resources selected for decomposition: {}", result);
        return result;
    }

    private Collection<DiscoverableEntity> selectFpgaProcessors(ComposedNode node) {
        return node.getProcessors().stream()
            .filter(ProcessorHelper::isRemoteFpga)
            .collect(toList());
    }

    private Collection<Endpoint> selectEndpoints(ComposedNode node) {
        return concat(
            findTargetEndpoints(node),
            findInitiatorEndpoints(node)
        ).collect(toSet());
    }

    private Collection<DiscoverableEntity> selectVolumes(ComposedNode node, Collection<Endpoint> allEndpoints) {
        return concat(
            allEndpoints.stream()
                .flatMap(endpoint -> endpoint.getConnectedEntities().stream())
                .map(ConnectedEntity::getEntityLink)
                .filter(Volume.class::isInstance),
            node.getVolumes().stream()
        ).filter(Objects::nonNull)
            .collect(toSet());
    }

    private Collection<DiscoverableEntity> selectZones(Collection<Endpoint> allEndpoints) {
        return allEndpoints.stream().map(Endpoint::getZone).filter(Objects::nonNull).collect(toSet());
    }

    private Stream<Endpoint> findInitiatorEndpoints(ComposedNode node) {
        return node.getComputerSystem().getEndpoints().stream();
    }

    private Stream<Endpoint> findTargetEndpoints(ComposedNode node) {
        return node.getEndpoints().stream();
    }
}
