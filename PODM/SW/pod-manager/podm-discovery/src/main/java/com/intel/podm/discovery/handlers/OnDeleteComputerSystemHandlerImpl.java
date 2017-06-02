/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.discovery.handlers;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.handlers.OnDeleteComputerSystemHandler;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.ComposedNodeUpdater;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;
import com.intel.podm.discovery.external.ExternalServiceRepository;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.RSS;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class OnDeleteComputerSystemHandlerImpl implements OnDeleteComputerSystemHandler {
    @Inject
    private GenericDao genericDao;

    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Inject
    private ComposedNodeUpdater composedNodeUpdater;

    @Override
    @Transactional(MANDATORY)
    public void preRemove(ComputerSystem computerSystem) {
        genericDao.remove(computerSystem.getMetadata());
        removeLuiProperties(computerSystem);
        triggerStorageServicesCheck();

        composedNodeUpdater.disableComposedNode(computerSystem.getComposedNode());
    }

    private void removeLuiProperties(ComputerSystem computerSystem) {
        genericDao.removeAndClear(computerSystem.getEthernetInterfaces(), ethernetInterface -> isLui(ethernetInterface.getService()));
    }

    private boolean isLui(ExternalService service) {
        return service == null || LUI.equals(service.getServiceType());
    }

    private void triggerStorageServicesCheck() {
        for (ExternalService service : externalServiceRepository.getAllByType(RSS)) {
            availabilityChecker.verifyServiceAvailabilityByUuid(service.getUuid());
        }
    }
}
