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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachActionValidator;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointFinder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.AttachResourceRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.val;

import java.util.Set;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContextBuilder.createAttachOperationContext;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;

@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public abstract class Attacher<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {

    static final String ATTACHER_BEAN_PREFIX = "Attacher_";

    private final AttachActionValidator<T> attachActionValidator;

    private final TargetEndpointFinder targetEndpointFinder;

    private final RemoteAttacher remoteAttacher;

    private final LocalAttacher localAttacher;

    public Attacher(AttachActionValidator<T> attachActionValidator, TargetEndpointFinder targetEndpointFinder,
                    RemoteAttacher remoteAttacher, LocalAttacher localAttacher) {

        this.attachActionValidator = attachActionValidator;
        this.targetEndpointFinder = targetEndpointFinder;
        this.remoteAttacher = remoteAttacher;
        this.localAttacher = localAttacher;
    }

    protected abstract Fabric retrieveFabricFromAsset(T asset);

    protected abstract Protocol retrieveAssetProtocol(T asset);

    protected abstract Violations getAssetSpecificViolations(ComposedNode composedNode, T asset);

    public void validate(ComposedNode composedNode, T assetToAttach, AttachResourceRequest request) throws BusinessApiException {
        attachActionValidator.validateAssetExists(assetToAttach, request.getResourceODataId());
        attachActionValidator.validateAssetPresence(assetToAttach, request.getResourceODataId());
        attachActionValidator.validateProtocolMatches(request.getProtocol(), retrieveAssetProtocol(assetToAttach));
        attachActionValidator.validateAssetAttachAbility(assetToAttach);
        attachActionValidator.checkAssetSpecificViolations(getAssetSpecificViolations(composedNode, assetToAttach));
    }

    public void attach(ComposedNode composedNode, T assetToAttach) throws BusinessApiException {
        val context = createAttachOperationContext(retrieveFabricFromAsset(assetToAttach), composedNode, assetToAttach);

        remoteAttacher.execute(this, context);
        localAttacher.execute(this, context);
    }

    protected ODataId getOrCreateTargetEndpoint(DiscoverableEntity assetToAttach, ODataId fabricODataId, Protocol fabricType) {
        return targetEndpointFinder.findOrCreateTargetEndpoint((DiscoverableEntity & AttachableAsset) assetToAttach, fabricODataId, fabricType);
    }

    protected void attachRelatedAssets(ComposedNode composedNode, Set<Endpoint> endpoints) {
        endpoints.stream()
            .filter(e -> e.hasRole(TARGET))
            .forEach(composedNode::attachAsset);
    }
}
