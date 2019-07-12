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

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.val;

public class Detacher<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {

    private final LocalDetacher<T> localDetacher;

    private final DetachRequestValidator<T> detachRequestValidator;

    private final RemoteDetacher<T> remoteDetacher;

    private final DetachContextProvider<T> detachContextProvider;

    public Detacher(DetachRequestValidator<T> detachRequestValidator,
                    RemoteDetacher<T> remoteDetacher,
                    LocalDetacher<T> localDetacher,
                    DetachContextProvider<T> detachContextProvider) {

        this.detachRequestValidator = detachRequestValidator;
        this.localDetacher = localDetacher;
        this.remoteDetacher = remoteDetacher;
        this.detachContextProvider = detachContextProvider;
    }

    public void detach(ComposedNode composedNode, T assetToBeDetached) throws BusinessApiException {

        detachRequestValidator.validate(composedNode, assetToBeDetached);

        val operationContext = detachContextProvider.createContext(assetToBeDetached, composedNode);

        remoteDetacher.execute(operationContext);

        localDetacher.execute(operationContext);
    }
}
