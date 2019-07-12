/*
 * Copyright (c) 2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Optional;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.isResourceNotFoundTheRootCause;
import static java.lang.String.format;

@Component
@Slf4j
public class RemoteDetachStrategyAdapter<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> implements RemoteDetachStrategy<T> {

    private final ZoneActionsInvoker zoneActionsInvoker;

    private final RestRequestInvoker restRequestInvoker;

    @Autowired
    public RemoteDetachStrategyAdapter(ZoneActionsInvoker zoneActionsInvoker, RestRequestInvoker restRequestInvoker) {
        this.zoneActionsInvoker = zoneActionsInvoker;
        this.restRequestInvoker = restRequestInvoker;
    }

    @Override
    public void detachOnRemoteFabric(T assetToBeDetached, AttachDetachOperationContext context) throws EntityOperationException {
        val targetEndpoint = findTargetEndpoint(assetToBeDetached);
        if (targetEndpoint.isPresent()) {
            val targetEndpointOdataId = targetEndpoint.get().getUri();
            val relatedZoneOdataId = targetEndpoint.get().getZone().getUri();

            zoneActionsInvoker.removeFromZone(targetEndpointOdataId, relatedZoneOdataId);

            // TODO: RSD-4999 - check connected entities before delete == 1
            removeEndpoint(targetEndpointOdataId);

            context.setTargetEndpointODataId(targetEndpointOdataId);
            context.setRelatedZoneODataId(relatedZoneOdataId);
        }
    }

    private void removeEndpoint(ODataId targetEndpointOdataId) throws EntityOperationException {
        try {
            restRequestInvoker.delete(targetEndpointOdataId.toUri());
        } catch (WebClientRequestException e) {
            mitigateEndpointNotFound(targetEndpointOdataId, e);
        }
    }

    private void mitigateEndpointNotFound(ODataId targetEndpointOdataId, WebClientRequestException e) throws EntityOperationException {
        if (isResourceNotFoundTheRootCause(e)) {
            log.warn(format("Unable to find endpoint for removal [path: %s], it might have been already removed.", targetEndpointOdataId.toUri()));
        } else {
            String errorMessage = format("Operation DELETE failed for endpoint [path: %s]", targetEndpointOdataId.toUri());
            log.warn(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    public void onAfterRemoteDetach(T resource) {
        //do nothing
    }

    private Optional<Endpoint> findTargetEndpoint(T assetToDetach) {
        // TODO: RSD-4999 - find endpoint from external service
        return optionalSingle(assetToDetach.getEndpoints());
    }
}
