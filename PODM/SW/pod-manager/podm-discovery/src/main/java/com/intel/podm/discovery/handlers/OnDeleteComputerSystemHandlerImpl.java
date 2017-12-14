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

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.handlers.OnDeleteComputerSystemHandler;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.InBandServiceConfig;
import com.intel.podm.discovery.ComposedNodeUpdater;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.util.Collections.singleton;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class OnDeleteComputerSystemHandlerImpl implements OnDeleteComputerSystemHandler {
    @Inject
    private GenericDao genericDao;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private ServiceExplorer serviceExplorer;

    @Inject
    private ComposedNodeUpdater composedNodeUpdater;

    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    @Config
    private Holder<InBandServiceConfig> inBandServiceConfigHolder;

    @Override
    @Transactional(MANDATORY)
    public void preRemove(ComputerSystem computerSystem) {
        removeAffectedEntitiesFromComplementaryService(computerSystem);
        genericDao.remove(computerSystem.getMetadata());
        removeEthernetInterfacesReadFromLui(computerSystem);
        removeDrivesReadFromLui(computerSystem);
        triggerStorageServicesCheck();

        composedNodeUpdater.disableComposedNode(computerSystem.getComposedNode());
    }

    private void removeAffectedEntitiesFromComplementaryService(ComputerSystem computerSystem) {
        if (!computerSystem.isComplementary() && inBandServiceConfigHolder.get().isInBandServiceSupportEnabled()) {
            List<ComputerSystem> inBandComputerSystems = computerSystemDao.findComplementarySystems(computerSystem);
            inBandComputerSystems.forEach(this::removeComplementaryComputerSystem);
        }
    }

    private void removeEthernetInterfacesReadFromLui(ComputerSystem computerSystem) {
        genericDao.removeAndClear(computerSystem.getEthernetInterfaces(), ethernetInterface -> isLui(ethernetInterface.getService()));
    }

    private void removeDrivesReadFromLui(ComputerSystem computerSystem) {
        genericDao.removeAndClear(getDrivesFromComputerSystemStorage(computerSystem), drive -> isLui(drive.getService()));
    }

    private Set<Drive> getDrivesFromComputerSystemStorage(ComputerSystem computerSystem) {
        return computerSystem.getStorages()
                             .stream()
                             .flatMap(storage -> storage.getDrives().stream())
                             .collect(toSet());
    }

    private boolean isLui(ExternalService service) {
        return service == null || LUI.equals(service.getServiceType());
    }

    private void triggerStorageServicesCheck() {
        for (ExternalService service : externalServiceDao.getExternalServicesByServicesTypes(singleton(RSS))) {
            serviceExplorer.enqueueVerification(service.getUuid());
        }
    }

    private void removeComplementaryComputerSystem(ComputerSystem computerSystem) {
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getEthernetInterfaces());
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getMemoryModules());
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getProcessors());
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getSimpleStorages());
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getStorages());
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem.getChassis(), Chassis::getDrives);
        discoverableEntityDao.removeWithConnectedExternalLinks(computerSystem);
    }
}
