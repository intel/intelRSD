/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.NonNull;

public final class AttachableContextBuilder<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {
    private ComposedNode composedNode;
    private T assetToAttach;
    private Fabric fabric;

    private AttachableContextBuilder(ComposedNode composedNode) {
        this.composedNode = composedNode;
    }

    public AttachableContextBuilder withFabric(@NonNull Fabric fabric) {
        this.fabric = fabric;
        return this;
    }

    public AttachableContextBuilder withAsset(@NonNull T assetToAttach) {
        this.assetToAttach = assetToAttach;
        return this;
    }

    public static AttachableContextBuilder newAttachableContextBuilder(@NonNull ComposedNode composedNode) {
        return new AttachableContextBuilder(composedNode);
    }

    public AttachableContext build() {
        return AttachableContext.builder().
            fabricODataId(fabric.getUri()).
            fabricType(fabric.getFabricType()).
            nodeODataId(composedNode.getUri()).
            assetODataId(assetToAttach.getUri()).
            build();
    }
}
