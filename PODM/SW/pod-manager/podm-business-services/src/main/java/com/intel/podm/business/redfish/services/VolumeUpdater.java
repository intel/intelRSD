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
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.redfish.RedfishVolume;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class VolumeUpdater {

    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(REQUIRES_NEW)
    public void updateVolume(Context volumeContext, RedfishVolume redfishVolume) throws BusinessApiException {
        Volume volume = (Volume) traverser.traverse(volumeContext);
        if (redfishVolume.getEraseOnDetach() != null) {
            volume.setEraseOnDetach(redfishVolume.getEraseOnDetach());
        }
    }
}
