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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortCreation;
import com.intel.podm.business.dto.redfish.RequestedEthernetSwitchPortModification;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortActionHelper;
import com.intel.podm.business.redfish.services.helpers.EthernetSwitchPortVlanHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.types.Id;

import javax.ejb.Stateless;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class EthernetSwitchPortActionsService {

    @Inject
    private GenericDao genericDao;

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private EthernetSwitchPortActionsInvoker invoker;

    @Inject
    private EthernetSwitchPortActionHelper switchPortHelper;

    @Inject
    private EthernetSwitchPortVlanHelper vlanHelper;

    @Inject
    private EthernetSwitchPortUpdater ethernetSwitchPortUpdater;

    @Transactional(REQUIRED)
    public Context createSwitchPort(Context switchContext, RequestedEthernetSwitchPortCreation requestedEthernetSwitchPortCreation)
            throws ActionException, EntityNotFoundException {

        EthernetSwitch currentSwitch = (EthernetSwitch) traverser.traverse(switchContext);

        if (!currentSwitch.isEnabledAndHealthy()) {
            throw new ActionException("EthernetSwitch should be enabled and healthy in order to invoke actions on it.");
        }

        Optional<List<EthernetSwitchPort>> switchPorts =
                switchPortHelper.validateAndGetSwitchPorts(currentSwitch.getId(), requestedEthernetSwitchPortCreation.getLinks());

        EthernetSwitchPort newSwitchPort =
                invoker.createSwitchPort(currentSwitch, switchPortHelper.switchPortDefinition(requestedEthernetSwitchPortCreation, switchPorts));
        return toContext(newSwitchPort);
    }

    @Transactional(REQUIRED)
    public void updateSwitchPort(Context switchPortContext, RequestedEthernetSwitchPortModification requestedEthernetSwitchPortModification)
            throws ActionException, EntityNotFoundException {

        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        Optional<List<EthernetSwitchPort>> switchPorts =
                switchPortHelper.validateAndGetSwitchPorts(switchPort.getEthernetSwitch().getId(), requestedEthernetSwitchPortModification.getLinks());
        URI vlanUri = vlanHelper.validateAndGetVlanUri(switchPort.getId(), requestedEthernetSwitchPortModification.getPrimaryVlan());

        invoker.updateSwitchPort(switchPort, switchPortHelper.switchPortRedefinition(requestedEthernetSwitchPortModification, switchPorts, vlanUri));
    }

    @Transactional(REQUIRED)
    public void deleteSwitchPort(Context switchPortContext) throws EntityNotFoundException, ActionException {
        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(switchPortContext);
        invoker.deleteSwitchPort(switchPort);
        ethernetSwitchPortUpdater.removeEthernetSwitchPortWithRetry(switchPort.getId());
    }

    @Dependent
    @Interceptors(RetryOnRollbackInterceptor.class)
    private static class EthernetSwitchPortUpdater {

        @Inject
        private GenericDao genericDao;

        EthernetSwitchPortUpdater() {
        }

        @NumberOfRetriesOnRollback(3)
        @Transactional(REQUIRES_NEW)
        public void removeEthernetSwitchPortWithRetry(Id ethernetSwitchPortId) throws IllegalStateException {
            EthernetSwitchPort port = genericDao.find(EthernetSwitchPort.class, ethernetSwitchPortId);
            port.getService().setIsDirty(true);
            genericDao.remove(port);
        }
    }
}

