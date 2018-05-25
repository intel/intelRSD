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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Objects;
import java.util.UUID;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toSet;
import static java.util.stream.Stream.concat;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class StorageServiceAssetsSelector {
    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    Collection<DiscoverableEntity> selectRelatedAssets(UUID serviceUuid, ComposedNode node) {
        Collection<Endpoint> allEndpoints = selectEndpoints(serviceUuid, node);
        Collection<DiscoverableEntity> allZones = selectZones(allEndpoints);
        Collection<DiscoverableEntity> allVolumes = selectVolumes(node, allEndpoints);
        ArrayList<DiscoverableEntity> result = new ArrayList<DiscoverableEntity>() {{
            addAll(allZones);
            addAll(allEndpoints);
            addAll(allVolumes);
        }};
        logger.d("Resources selected for decomposition: {}", result);
        return result;
    }

    private Collection<Endpoint> selectEndpoints(UUID serviceUuid, ComposedNode node) {
        return concat(
            findTargetEndpoints(serviceUuid, node),
            findInitiatorEndpoints(serviceUuid, node)
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

    private Stream<Endpoint> findInitiatorEndpoints(UUID serviceUuid, ComposedNode node) {
        return node.getComputerSystem().getEndpoints().stream()
            .filter(endpoint -> Objects.equals(endpoint.getService().getUuid(), serviceUuid));
    }

    private Stream<Endpoint> findTargetEndpoints(UUID serviceUuid, ComposedNode node) {
        return node.getEndpoints().stream()
            .filter(endpoint -> Objects.equals(endpoint.getService().getUuid(), serviceUuid));
    }
}
