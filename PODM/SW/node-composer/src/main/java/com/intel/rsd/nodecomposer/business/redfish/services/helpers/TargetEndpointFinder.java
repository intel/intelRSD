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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.RemoteEndpointAccessService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.SneakyThrows;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.badRequestInTheRootCause;
import static java.lang.String.format;

@Component
public class TargetEndpointFinder {

    private final RemoteEndpointAccessService remoteEndpointAccessService;

    @Autowired
    public TargetEndpointFinder(RemoteEndpointAccessService remoteEndpointAccessService) {
        this.remoteEndpointAccessService = remoteEndpointAccessService;
    }

    public <T extends DiscoverableEntity & AttachableAsset> ODataId findOrCreateTargetEndpoint(T assetToAttach, ODataId fabricODataId, Protocol fabricType) {
        val endpoint = optionalSingle(assetToAttach.getEndpoints()).map(DiscoverableEntity::getUri);
        return endpoint.orElseGet(() -> createOrObtainTargetEndpoint(fabricODataId, fabricType, assetToAttach.getUri()));
    }

    @SneakyThrows
    private ODataId createOrObtainTargetEndpoint(ODataId fabricODataId, Protocol fabricType, ODataId assetToAttachUri) {
        try {
            return remoteEndpointAccessService.createTargetEndpoint(fabricODataId, fabricType, assetToAttachUri);
        } catch (EntityOperationException e) {
            if (badRequestInTheRootCause(e)) {
                return oDataIdFromUri(remoteEndpointAccessService.getTargetEndpointUriByRelatedAssetUri(fabricODataId, assetToAttachUri));
            }
            throw new EntityOperationException(
                format("Cannot find target endpoint on fabric(%s) which is related with requested asset(%s)", fabricODataId, assetToAttachUri), e
            );
        }
    }
}
