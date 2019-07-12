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
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.val;

/**
 * This class SHOULD NOT execute any WRITES on DB. Responsibility of this class is to perform all required remote operations (REST RW).
 */
public class RemoteDetacher<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {

    private final EntityTreeTraverser traverser;

    private final RemoteDetachStrategy<T> remoteDetachStrategy;

    public RemoteDetacher(EntityTreeTraverser entityTreeTraverser, RemoteDetachStrategy<T> remoteDetachStrategy) {
        this.traverser = entityTreeTraverser;
        this.remoteDetachStrategy = remoteDetachStrategy;
    }

    @SuppressWarnings("unchecked")
    public void execute(AttachDetachOperationContext context) throws BusinessApiException {

        val assetToBeDetached = (T) traverser.traverseDiscoverableEntity(context.getAssetODataId());

        remoteDetachStrategy.detachOnRemoteFabric(assetToBeDetached, context);

        remoteDetachStrategy.onAfterRemoteDetach(assetToBeDetached);
    }
}
