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
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContext;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.EndpointObtainer;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Set;

import static com.google.common.collect.ImmutableSet.of;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class UpdateNodeAssetsTask implements RequiresNewTransactionWrapper.VoidTask<BusinessApiException> {
    private final ZoneObtainer zoneObtainer;
    private final GenericDao genericDao;
    private final EndpointObtainer endpointObtainer;

    private Attacher attachStrategy;
    private AttachableContext context;

    @Autowired
    public UpdateNodeAssetsTask(ZoneObtainer zoneObtainer, GenericDao genericDao, EndpointObtainer endpointObtainer) {
        this.zoneObtainer = zoneObtainer;
        this.genericDao = genericDao;
        this.endpointObtainer = endpointObtainer;
    }

    @Override
    public void execute() throws BusinessApiException {
        val endpoints = discoverAndReturnRelatedEndpoints();
        discoverRelatedZone();
        val node = genericDao.find(ComposedNode.class, context.getNodeODataId());
        val asset = genericDao.find(DiscoverableEntity.class, context.getAssetODataId());
        node.attachAsset((AttachableAsset) asset);
        attachStrategy.attachRelatedAssets(node, endpoints);
    }

    public UpdateNodeAssetsTask init(Attacher attachStrategy, AttachableContext context) {
        this.attachStrategy = attachStrategy;
        this.context = context;
        return this;
    }

    private void discoverRelatedZone() throws BusinessApiException {
        zoneObtainer.discoverZone(context.getFabricODataId().toUri(), context.getRelatedZoneODataId().toUri());
    }

    private Set<Endpoint> discoverAndReturnRelatedEndpoints() throws BusinessApiException {
        val targetEndpoint = discoverEndpoint(context.getTargetEndpointODataId());
        val initiatorEndpoint = discoverEndpoint(context.getInitiatorEndpointODataId());
        return of(targetEndpoint, initiatorEndpoint);
    }

    private Endpoint discoverEndpoint(ODataId targetEndpointODataId) throws BusinessApiException {
        try {
            val fabric = genericDao.find(Fabric.class, context.getFabricODataId());
            return endpointObtainer.discoverEndpoint(fabric, targetEndpointODataId.toUri());
        } catch (WebClientRequestException e) {
            String errorMessage = "Discover failed";
            log.info(errorMessage + " on [ ODataId: {} ]", targetEndpointODataId);
            throw new BusinessApiException(errorMessage, e);
        }
    }
}
