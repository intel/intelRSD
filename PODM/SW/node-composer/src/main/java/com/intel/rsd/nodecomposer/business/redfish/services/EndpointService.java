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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.UUID;
import java.util.function.Consumer;

import static org.springframework.transaction.annotation.Propagation.REQUIRES_NEW;

@Slf4j
@Service
public class EndpointService {

    private final RemoteEndpointAccessService remoteEndpointAccessService;
    private final EndpointSynchronizationService endpointSynchronizationService;

    @Autowired
    public EndpointService(RemoteEndpointAccessService remoteEndpointAccessService, EndpointSynchronizationService endpointSynchronizationService) {
        this.remoteEndpointAccessService = remoteEndpointAccessService;
        this.endpointSynchronizationService = endpointSynchronizationService;
    }

    public ODataId createTargetEndpoint(ODataId fabricOdataId, Protocol fabricType, ODataId targetOdataId) throws BusinessApiException {
        return remoteEndpointAccessService.createTargetEndpoint(fabricOdataId, fabricType, targetOdataId);
    }

    public ODataId createInitiatorEndpoint(ODataId fabricOdataId, Protocol fabricType,
                                           UUID targetSystemUuid, List<String> targetSystemConnectionIds) throws BusinessApiException {
        return remoteEndpointAccessService.createInitiatorEndpoint(fabricOdataId, fabricType, targetSystemUuid, targetSystemConnectionIds);
    }

    @Transactional(propagation = REQUIRES_NEW, rollbackFor = EntityOperationException.class)
    public void discoverEndpoint(ODataId fabricOdataId, ODataId endpointOdataId, Consumer<? super Endpoint> callback) throws EntityOperationException {
        val createdEndpoint = endpointSynchronizationService.discoverEndpoint(fabricOdataId, endpointOdataId);
        callback.accept(createdEndpoint);
    }

}
