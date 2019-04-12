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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.HashSet;
import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component(ENDPOINT)
@Scope(SCOPE_SINGLETON)
public class EndpointDetacher extends Detacher<Endpoint> {
    private final DriveDetacher driveDetacher;

    @Autowired
    public EndpointDetacher(DriveDetacher driveDetacher) {
        this.driveDetacher = driveDetacher;
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Endpoint endpoint) {
        return endpoint.getFabric();
    }

    @Override
    protected void validate(ComposedNode composedNode, Endpoint endpoint) throws RequestValidationException {
        if (!composedNode.getEndpoints().contains(endpoint)) {
            throw new RequestValidationException(createWithViolations("Provided endpoint is not attached to this Node."));
        }

        if (ISCSI.equals(endpoint.getProtocol())) {
            throw new RequestValidationException(createWithViolations("Detach endpoint with iSCSI protocol is not allowed."));
        }
    }

    @Override
    protected void detachRelatedItems(ComposedNode composedNode, Endpoint endpoint) {
        performSecureEraseOnDrive(composedNode, endpoint.getDrives());
        collectEndpointRelatedAssets(endpoint).forEach(composedNode::detachAsset);
    }

    @Override
    protected void performResourceSpecificActions(ComposedNode composedNode, Endpoint endpoint) {
    }

    @Override
    protected void removeResource(ODataId targetEndpointODataId) {
    }

    @Override
    protected Optional<Endpoint> retrieveTargetEndpoint(AttachableAsset assetToDetach) {
        return Optional.ofNullable((Endpoint) assetToDetach);
    }

    private Set<AttachableAsset> collectEndpointRelatedAssets(Endpoint endpoint) {
        Set<AttachableAsset> endpointRelatedAssets = new HashSet<>(endpoint.getProcessors());
        endpointRelatedAssets.addAll(endpoint.getVolumes());
        endpointRelatedAssets.addAll(endpoint.getDrives());

        return endpointRelatedAssets;
    }

    private void performSecureEraseOnDrive(ComposedNode composedNode, Collection<Drive> drives) {
        drives.forEach(d -> driveDetacher.performResourceSpecificActions(composedNode, d));
    }
}
