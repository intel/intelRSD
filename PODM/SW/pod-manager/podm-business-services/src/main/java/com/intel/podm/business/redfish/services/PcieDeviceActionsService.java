/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.PcieDeviceUpdateInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.PcieDeviceUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishPcieDevice;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class PcieDeviceActionsService {
    @Inject
    private PcieDeviceUpdateInvoker invoker;

    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(REQUIRES_NEW)
    public void updatePcieDeviceAssetTag(Context pcieDeviceContext, RedfishPcieDevice representation) throws BusinessApiException {
        PcieDeviceUpdateDefinition pcieDeviceUpdateDefinition = new PcieDeviceUpdateDefinition(representation.getAssetTag());
        PcieDevice pcieDevice = (PcieDevice) traverser.traverse(pcieDeviceContext);
        invoker.updatePcieDevice(pcieDevice, pcieDeviceUpdateDefinition);
    }
}
