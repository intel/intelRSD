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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachActionValidator;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointFinder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.HashSet;
import java.util.Set;
import java.util.stream.Stream;

import static com.intel.rsd.nodecomposer.business.redfish.services.attach.Attacher.ATTACHER_BEAN_PREFIX;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;

@Component(ATTACHER_BEAN_PREFIX + ENDPOINT)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EndpointAttacher extends Attacher<Endpoint> {

    public EndpointAttacher(AttachActionValidator<Endpoint> attachActionValidator, TargetEndpointFinder targetEndpointFinder,
                            RemoteAttacher remoteAttacher, LocalAttacher localAttacher) {

        super(attachActionValidator, targetEndpointFinder, remoteAttacher, localAttacher);
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Endpoint endpoint) {
        return endpoint.getFabric();
    }

    @Override
    protected Protocol retrieveAssetProtocol(Endpoint endpoint) {
        return endpoint.getProtocol();
    }

    @Override
    public Violations getAssetSpecificViolations(ComposedNode composedNode, Endpoint endpoint) {
        val violations = new Violations();

        val connectedEntities = endpoint.getConnectedEntities();
        if (connectedEntities == null || connectedEntities.isEmpty()) {
            violations.addViolation("Selected Endpoint should have at least one ConnectedEntity.");
        }

        if (!endpoint.hasRole(TARGET)) {
            violations.addViolation("Selected Endpoint should be of type target.");
        }

        return violations;
    }

    @Override
    protected ODataId getOrCreateTargetEndpoint(DiscoverableEntity assetToAttach, ODataId fabricODataId, Protocol fabricType) {
        return assetToAttach.getUri();
    }

    @Override
    protected void attachRelatedAssets(ComposedNode composedNode, Set<Endpoint> endpoints) {
        endpoints.stream()
            .filter(e -> e.hasRole(TARGET))
            .flatMap(this::collectEndpointRelatedAssets)
            .forEach(composedNode::attachAsset);
    }

    private Stream<AttachableAsset> collectEndpointRelatedAssets(Endpoint endpoint) {
        val endpointRelatedAssets = new HashSet<AttachableAsset>(endpoint.getProcessors());
        endpointRelatedAssets.addAll(endpoint.getVolumes());
        endpointRelatedAssets.addAll(endpoint.getDrives());
        return endpointRelatedAssets.stream();
    }
}
