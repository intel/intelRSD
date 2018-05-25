/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.PcieZoneActionsInvoker;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.PcieLocalStorage;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.Optional;

import static com.intel.podm.business.Violations.createWithViolations;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PcieZoneAttacher {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private LocalStorageCollector localStorageCollector;

    @Inject
    private PcieZoneActionsInvoker pcieZoneActionsInvoker;

    @Transactional(REQUIRES_NEW)
    public void attachEndpointToZone(Context composedNodeContext, Context pcieDriveContext) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);

        Drive pcieDrive = (Drive) traverser.traverse(pcieDriveContext);
        Optional<PcieLocalStorage> pcieLocalStorage = findPcieLocalStorageByResource(composedNode.getComputerSystem(), pcieDrive);
        PcieLocalStorage localStorage = pcieLocalStorage
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Provided PCIeDrive cannot be attached to this Node.")));

        pcieZoneActionsInvoker.attachEndpoint(localStorage.getZone(), localStorage.getEndpoint());
    }

    private Optional<PcieLocalStorage> findPcieLocalStorageByResource(ComputerSystem computerSystem, Drive drive) {
        return localStorageCollector.getPcieStorage(computerSystem).stream()
            .filter(pcieLocalStorage -> Objects.equals(pcieLocalStorage.getDrive(), drive))
            .findFirst();
    }
}
