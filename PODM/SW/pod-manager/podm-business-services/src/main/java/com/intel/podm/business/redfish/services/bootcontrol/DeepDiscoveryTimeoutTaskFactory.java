/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.bootcontrol;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.UUID;

@Dependent
class DeepDiscoveryTimeoutTaskFactory {

    @Inject
    private BeanFactory factory;

    public DeepDiscoveryTimeoutTask create(Id computerSystemId, UUID deepDiscoveryTaskUuid) {
        DeepDiscoveryTimeoutTask deepDiscoveryTimeoutTask = factory.create(DeepDiscoveryTimeoutTask.class);
        deepDiscoveryTimeoutTask.setComputerSystemId(computerSystemId);
        deepDiscoveryTimeoutTask.setTaskUuid(deepDiscoveryTaskUuid);
        return deepDiscoveryTimeoutTask;
    }
}
