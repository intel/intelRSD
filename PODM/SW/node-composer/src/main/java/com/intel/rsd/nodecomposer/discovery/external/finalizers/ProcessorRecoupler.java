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
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class ProcessorRecoupler extends Recoupler<Processor> {
    @Override
    protected void reattach(ComposedNode node, Collection<Processor> assets) {
        val associatedProcessorUris = node.getAssociatedProcessorsUris();

        val processorsToReAttach = assets.stream()
            .filter(processor -> associatedProcessorUris.contains(processor.getUri().toUri()))
            .collect(toList());

        if (!associatedProcessorUris.isEmpty() && processorsToReAttach.isEmpty()) {
            log.warn(format("Processors with URIs %s associated with composed node were not found.", associatedProcessorUris));
            return;
        }

        for (Processor processor : processorsToReAttach) {
            node.attachAsset(processor);
        }
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return node.getAssociatedProcessorsUris().isEmpty()
            || composedNodeProcessorsCollectionIsConsistent(node);
    }

    private boolean composedNodeProcessorsCollectionIsConsistent(ComposedNode node) {
        return !node.getProcessors().isEmpty()
            && allProcessorsAreConnectedToNode(node)
            && allProcessorsAreHealthyAndConnectedWithEndpoints(node);
    }

    private boolean allProcessorsAreConnectedToNode(ComposedNode node) {
        return node.getAssociatedProcessorsUris().size() == node.getProcessors().size()
            && composedNodeContainsAllAssociatedProcessors(node);
    }

    private boolean composedNodeContainsAllAssociatedProcessors(ComposedNode node) {
        Set<URI> composedNodeProcessorsUris = node.getProcessors().stream()
            .filter(Processor::isAchievable)
            .map(processor -> processor.getUri().toUri())
            .collect(toSet());

        return composedNodeProcessorsUris.containsAll(node.getAssociatedProcessorsUris());
    }

    private boolean allProcessorsAreHealthyAndConnectedWithEndpoints(ComposedNode composedNode) {
        return composedNode.getProcessors().stream().allMatch(processor ->
            statusOf(processor).isEnabled().isHealthy().verify()
                && isNotEmpty(processor.getEntityConnections())
                && processor.getEntityConnections().stream()
                .noneMatch(connectedEntity -> isNull(connectedEntity.getEndpoint()) && TARGET.equals(connectedEntity.getEntityRole()))
        );
    }
}
