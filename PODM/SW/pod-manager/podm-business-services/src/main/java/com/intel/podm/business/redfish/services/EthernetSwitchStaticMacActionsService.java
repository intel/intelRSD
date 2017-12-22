/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchStaticMac;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchStaticMacActionsInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchStaticMac;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class EthernetSwitchStaticMacActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetSwitchStaticMacActionsInvoker invoker;

    @Transactional(REQUIRES_NEW)
    public Context createStaticMac(Context context, RedfishEthernetSwitchStaticMac representation) throws BusinessApiException {
        EthernetSwitchPort currentPort = (EthernetSwitchPort) traverser.traverse(context);

        if (!currentPort.isEnabledAndHealthy()) {
            throw new ResourceStateMismatchException("EthernetSwitchPort should be enabled and healthy in order to invoke actions on it.");
        }

        EthernetSwitchStaticMac newStaticMac = invoker.createStaticMac(currentPort, representation);

        return toContext(newStaticMac);
    }

    @Transactional(REQUIRES_NEW)
    public void deleteStaticMac(Context target) throws BusinessApiException {
        EthernetSwitchStaticMac ethernetSwitchStaticMac = (EthernetSwitchStaticMac) traverser.traverse(target);
        invoker.delete(ethernetSwitchStaticMac);
    }

    @Transactional(REQUIRES_NEW)
    public void update(Context target, RedfishEthernetSwitchStaticMac representation) throws BusinessApiException {
        EthernetSwitchStaticMac staticMac = (EthernetSwitchStaticMac) traverser.traverse(target);
        invoker.update(staticMac, representation);
    }
}
