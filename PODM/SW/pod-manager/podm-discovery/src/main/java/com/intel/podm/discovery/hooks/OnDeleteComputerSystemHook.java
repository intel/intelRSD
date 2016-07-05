/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.hooks;


import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.hooks.OnDeleteHook;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.properties.Boot;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;
import com.intel.podm.discovery.external.ExternalServiceRepository;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.util.Objects.isNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class OnDeleteComputerSystemHook implements OnDeleteHook {
    @Inject
    private GenericDao genericDao;

    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    public void execute(DomainObject domainObject) {
        ComputerSystem system = (ComputerSystem) domainObject;
        removeConnectedProperties(system);
        removeLuiProperties(system);
        triggerStorageServicesCheck();
    }

    @Override
    public boolean canExecute(DomainObject domainObject) {
        return domainObject instanceof ComputerSystem;
    }

    private void removeConnectedProperties(ComputerSystem system) {
        Boot boot = system.getBoot();
        if (boot != null) {
            genericDao.remove(boot);
        }
        system.getPciDevices().forEach(genericDao::remove);
    }

    private void removeLuiProperties(ComputerSystem system) {
        removeLuiAdapters(system);
        removeLuiEthernetInterfaces(system);
    }

    private void removeLuiEthernetInterfaces(ComputerSystem system) {
        system.getEthernetInterfaces().stream()
                .filter(ethernetInterface -> isLui(ethernetInterface.getService()))
                .forEach(genericDao::remove);
    }

    private void removeLuiAdapters(ComputerSystem system) {
        system.getAdapters().stream()
                .filter(adapter -> isLui(adapter.getService()))
                .forEach(adapter -> {
                    adapter.getDevices().forEach(genericDao::remove);
                    genericDao.remove(adapter);
                });
    }

    private void triggerStorageServicesCheck() {
        for (ExternalService service : externalServiceRepository.getAllByType(RSS)) {
            availabilityChecker.verifyServiceAvailabilityByUuid(service.getUuid());
        }
    }

    private boolean isLui(ExternalService service) {
        return isNull(service) || LUI.equals(service.getServiceType());
    }
}
