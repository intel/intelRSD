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

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class LocalDetachStrategyAdapter<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> implements LocalDetachStrategy<T> {

    private final GenericDao genericDao;

    private final ZoneObtainer zoneObtainer;

    @Autowired
    public LocalDetachStrategyAdapter(GenericDao genericDao, ZoneObtainer zoneObtainer) {
        this.genericDao = genericDao;
        this.zoneObtainer = zoneObtainer;
    }

    @Override
    public void deallocate(ComposedNode composedNode, T resource) {
        composedNode.deallocate(resource);
    }

    @Override
    public void updateFabricModel(ComposedNode composedNode, T resource, AttachDetachOperationContext context) throws BusinessApiException {

        for (Endpoint endpoint : resource.getEndpoints()) {
            composedNode.deallocate(endpoint);
            genericDao.remove(endpoint);
        }

        if (context.getFabricODataId() != null && context.getRelatedZoneODataId() != null) {
            zoneObtainer.discoverZone(context.getFabricODataId().toUri(), context.getRelatedZoneODataId().toUri());
        }
    }

    public void onAfterLocalDetach(ComposedNode composedNode, T resource) {
        //do nothing
    }
}
