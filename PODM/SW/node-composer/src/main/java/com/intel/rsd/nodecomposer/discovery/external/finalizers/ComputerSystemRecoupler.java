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
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Objects;
import java.util.UUID;

import static com.intel.rsd.collections.IterableHelper.singleOrNull;
import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class ComputerSystemRecoupler extends Recoupler<ComputerSystem> {
    @Override
    protected void reattach(ComposedNode node, Collection<ComputerSystem> assets) {
        UUID associatedComputerSystemUuid = node.getAssociatedComputerSystemUuid();

        requiresNonNull(associatedComputerSystemUuid, "computerSystemUuid");

        try {
            reattachComputerSystem(node, assets, associatedComputerSystemUuid);
        } catch (IllegalStateException e) {
            log.error(format("There is more than one computer system with UUID: %s",
                associatedComputerSystemUuid));
        }
    }

    @Override
    protected boolean verify(ComposedNode node) {
        ComputerSystem computerSystem = node.getComputerSystem();
        return computerSystem != null && computerSystem.isAchievable() && statusOf(computerSystem).isEnabled().isHealthy().verify();
    }

    private void reattachComputerSystem(ComposedNode composedNode, Collection<ComputerSystem> systems,
                                        UUID associatedComputerSystemUuid) {

        ComputerSystem associatedComputerSystem = singleOrNull(systems.stream()
            .filter(computerSystem -> statusOf(computerSystem).isEnabled().isHealthy().verify())
            .filter(computerSystem -> computerSystem.getUuid() != null)
            .filter(computerSystem -> Objects.equals(computerSystem.getUuid(), associatedComputerSystemUuid))
            .collect(toList()));

        if (associatedComputerSystem != null) {
            associatedComputerSystem.getMetadata().setAllocated(true);
            composedNode.setComputerSystem(associatedComputerSystem);
        }
    }
}
