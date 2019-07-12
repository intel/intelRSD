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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static java.util.Collections.emptySet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Slf4j
public class ZoneDisassembler extends EntityDisassembler<Zone> {
    private final ZoneActionsInvoker zoneActionsInvoker;

    @Autowired
    public ZoneDisassembler(ZoneActionsInvoker zoneActionsInvoker) {
        this.zoneActionsInvoker = zoneActionsInvoker;
    }

    @Override
    public void deallocate() {
        //do nothing
    }

    @Override
    void unlink() {
        //do nothing
    }

    @Override
    public void decomposeRemotely() throws WebClientRequestException {
        if (isExisting()) {
            try {
                log.debug("Updating zone: {}", entity);
                zoneActionsInvoker.updateZone(entity, emptySet());
                super.decomposeRemotely();
            } catch (BusinessApiException e) {
                throw new RuntimeException("Detaching endpoints from zone has failed.", e);
            }
        }
    }
}
