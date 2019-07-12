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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.EndpointSynchronizationService;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import java.util.Set;

import static com.google.common.collect.ImmutableSet.of;
import static org.springframework.transaction.annotation.Propagation.REQUIRES_NEW;

@Slf4j
@Component
public class LocalAttacher {
    private final ZoneObtainer zoneObtainer;
    private final GenericDao genericDao;
    private final EndpointSynchronizationService endpointSynchronizationService;

    @Autowired
    public LocalAttacher(ZoneObtainer zoneObtainer, GenericDao genericDao, EndpointSynchronizationService endpointSynchronizationService) {
        this.genericDao = genericDao;
        this.zoneObtainer = zoneObtainer;
        this.endpointSynchronizationService = endpointSynchronizationService;
    }

    @RetryOnRollback(3)
    @Transactional(propagation = REQUIRES_NEW, rollbackFor = BusinessApiException.class)
    public void execute(Attacher attachStrategy, AttachDetachOperationContext context) throws BusinessApiException {
        val fabricOdataId = context.getFabricODataId();
        val targetEndpointOdataId = context.getTargetEndpointODataId();
        val initiatorEndpointOdataId = context.getInitiatorEndpointODataId();

        val endpoints = discoverAndReturnRelatedEndpoints(fabricOdataId, targetEndpointOdataId, initiatorEndpointOdataId);
        zoneObtainer.discoverZone(context.getFabricODataId().toUri(), context.getRelatedZoneODataId().toUri());
        val node = genericDao.find(ComposedNode.class, context.getNodeODataId());
        val asset = genericDao.find(DiscoverableEntity.class, context.getAssetODataId());
        node.attachAsset((AttachableAsset) asset);
        attachStrategy.attachRelatedAssets(node, endpoints);
        node.getComputerSystem().addEndpoint(genericDao.find(Endpoint.class, initiatorEndpointOdataId));
    }

    private Set<Endpoint> discoverAndReturnRelatedEndpoints(ODataId fabricOdataId,
                                                            ODataId targetEndpointOdataId,
                                                            ODataId initiatorEndpointODataId) throws BusinessApiException {

        val targetEndpoint = discoverEndpoint(fabricOdataId, targetEndpointOdataId);
        val initiatorEndpoint = discoverEndpoint(fabricOdataId, initiatorEndpointODataId);
        return of(targetEndpoint, initiatorEndpoint);
    }

    private Endpoint discoverEndpoint(ODataId fabricOdataId, ODataId targetEndpointODataId) throws BusinessApiException {
        return endpointSynchronizationService.discoverEndpoint(fabricOdataId, targetEndpointODataId);
    }
}
