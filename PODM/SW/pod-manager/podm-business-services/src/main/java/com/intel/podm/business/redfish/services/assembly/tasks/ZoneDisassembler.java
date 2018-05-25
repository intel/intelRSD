/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.podm.client.WebClientRequestException;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.Collections.emptySet;

@Dependent
public class ZoneDisassembler extends EntityDisassembler<Zone> {

    @Inject
    private ZoneActionsInvoker zoneActionsInvoker;

    @Override
    public void deallocate() {
        //do nothing
    }

    @Override
    void unlink() {
        //do nothing
    }

    @Override
    public void removeRemoteAsset() throws WebClientRequestException {
        try {
            zoneActionsInvoker.updateZone(entity, emptySet());
        } catch (BusinessApiException e) {
            throw new RuntimeException("Detaching endpoints from zone has failed.", e);
        }
        super.removeRemoteAsset();
    }

}
