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

import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContext;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class DetachNodeAssetsTask implements RequiresNewTransactionWrapper.VoidTask<RuntimeException> {
    private final ZoneObtainer zoneObtainer;
    private final GenericDao genericDao;

    private Detacher detachStrategy;
    private AttachableContext context;

    @Autowired
    public DetachNodeAssetsTask(ZoneObtainer zoneObtainer, GenericDao genericDao) {
        this.zoneObtainer = zoneObtainer;
        this.genericDao = genericDao;
    }

    @Override
    @SneakyThrows
    public void execute() {
        val composedNode = genericDao.find(ComposedNode.class, context.getNodeODataId());
        val asset = genericDao.find(DiscoverableEntity.class, context.getAssetODataId());
        detachAssetContext(asset, composedNode);
        detachStrategy.detachRelatedItems(composedNode, asset);
        if (context.getFabricODataId() != null && context.getRelatedZoneODataId() != null) {
            zoneObtainer.discoverZone(context.getFabricODataId().toUri(), context.getRelatedZoneODataId().toUri());
        }
    }

    public DetachNodeAssetsTask init(Detacher detachStrategy, AttachableContext context) {
        this.detachStrategy = detachStrategy;
        this.context = context;
        return this;
    }

    private void detachAssetContext(DiscoverableEntity assetToDetach, ComposedNode composedNode) {
        val asset = (AttachableAsset) assetToDetach;
        asset.setAllocated(false);
        composedNode.detachAsset(asset);
    }
}
