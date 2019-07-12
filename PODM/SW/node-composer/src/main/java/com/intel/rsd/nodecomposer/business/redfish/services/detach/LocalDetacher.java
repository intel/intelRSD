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
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import lombok.val;
import org.springframework.transaction.annotation.Transactional;

import static org.springframework.transaction.annotation.Propagation.REQUIRES_NEW;

public class LocalDetacher<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {

    private GenericDao genericDao;

    private LocalDetachStrategy<T> localDetachStrategy;

    public LocalDetacher(GenericDao genericDao, LocalDetachStrategy<T> localDetachStrategy) {
        this.genericDao = genericDao;
        this.localDetachStrategy = localDetachStrategy;
    }

    @SuppressWarnings("unchecked")
    @Transactional(propagation = REQUIRES_NEW, rollbackFor = BusinessApiException.class)
    @RetryOnRollback(15)
    public void execute(AttachDetachOperationContext context) throws BusinessApiException {

        val composedNode = genericDao.find(ComposedNode.class, context.getNodeODataId());

        val resource = (T) genericDao.find(DiscoverableEntity.class, context.getAssetODataId());

        localDetachStrategy.deallocate(composedNode, resource);

        localDetachStrategy.updateFabricModel(composedNode, resource, context);

        localDetachStrategy.onAfterLocalDetach(composedNode, resource);
    }
}
