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
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Objects.isNull;
import static org.apache.commons.collections4.CollectionUtils.isNotEmpty;

@Dependent
class VolumeRecoupler extends Recoupler<Volume> {
    @Inject
    private Logger logger;

    @Override
    protected void reattach(ComposedNode node, Collection<Volume> assets) {
        UUID associatedStorageServiceUuid = node.getAssociatedStorageServiceUuid();
        Set<Identifier> associatedVolumeIdentifiers = node.getAssociatedEndpointIdentifiers();

        Volume volumeToReAttach = assets.stream()
            .filter(volume -> volume.getService().getUuid() != null)
            .filter(volume -> Objects.equals(volume.getService().getUuid(), associatedStorageServiceUuid))
            .filter(volume -> associatedVolumeIdentifiers.containsAll(volume.getIdentifiers()))
            .findFirst()
            .orElse(null);

        if (volumeToReAttach != null) {
            volumeToReAttach.getMetadata().setAllocated(true);
            node.addVolume(volumeToReAttach);
        } else {
            logger.w(format("Endpoint with id %s associated with composed node has not been found, on service with UUID: %s",
                associatedVolumeIdentifiers, associatedStorageServiceUuid));
        }
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return !composedNodeContainsAssociatedVolumes(node) || composedNodeContainsEnabledAndHealthyVolumes(node);
    }

    private boolean composedNodeContainsAssociatedVolumes(ComposedNode composedNode) {
        return composedNode.getAssociatedStorageServiceUuid() != null
            && !composedNode.getAssociatedVolumeIdentifiers().isEmpty();
    }

    private boolean composedNodeContainsEnabledAndHealthyVolumes(ComposedNode node) {
        return !node.getVolumes().isEmpty()
            && allVolumesAreHealthyAndConnectedWithEndpoints(node);
    }

    private boolean allVolumesAreHealthyAndConnectedWithEndpoints(ComposedNode composedNode) {
        return composedNode.getVolumes().stream().allMatch(volume ->
            statusOf(volume).isEnabled().isHealthy().verify()
            && isNotEmpty(volume.getEntityConnections())
            && volume.getEntityConnections().stream()
            .noneMatch(connectedEntity -> isNull(connectedEntity.getEndpoint()) && TARGET.equals(connectedEntity.getEntityRole()))
        );
    }
}
