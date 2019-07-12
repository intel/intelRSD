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
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.NonNull;

public final class AttachableContextBuilder {

    public static <T extends DiscoverableEntity> AttachDetachOperationContext createDetachOperationContext(@NonNull Fabric fabric,
                                                                                                           @NonNull ComposedNode composedNode,
                                                                                                           @NonNull T asset) {
        return createOperationContext(fabric, composedNode, asset);
    }

    public static <T extends DiscoverableEntity> AttachDetachOperationContext createAttachOperationContext(@NonNull Fabric fabric,
                                                                                                           @NonNull ComposedNode composedNode,
                                                                                                           @NonNull T asset) {
        return createOperationContext(fabric, composedNode, asset);
    }

    private static <T extends DiscoverableEntity> AttachDetachOperationContext createOperationContext(@NonNull Fabric fabric,
                                                                                                      @NonNull ComposedNode composedNode,
                                                                                                      @NonNull T asset) {
        return AttachDetachOperationContext.builder().
            fabricODataId(fabric.getUri()).
            fabricType(fabric.getFabricType()).
            nodeODataId(composedNode.getUri()).
            assetODataId(asset.getUri()).
            build();
    }
}
