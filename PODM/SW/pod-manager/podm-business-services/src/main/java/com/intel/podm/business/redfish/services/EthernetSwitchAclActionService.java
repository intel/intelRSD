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
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchAclActionsInvoker;
import com.intel.podm.business.services.context.Context;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class EthernetSwitchAclActionService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetSwitchAclActionsInvoker invoker;

    @Transactional(REQUIRES_NEW)
    public Context createAcl(Context switchContext) throws BusinessApiException {
        EthernetSwitch currentSwitch = (EthernetSwitch) traverser.traverse(switchContext);

        if (!statusOf(currentSwitch).isEnabled().isHealthy().verify()) {
            throw new ResourceStateMismatchException("EthernetSwitch should be enabled and healthy in order to invoke actions on it.");
        }

        EthernetSwitchAcl newAcl = invoker.createAcl(currentSwitch);
        return toContext(newAcl);
    }

    @Transactional(REQUIRES_NEW)
    public void deleteAcl(Context target) throws BusinessApiException {
        EthernetSwitchAcl ethernetSwitchAcl = (EthernetSwitchAcl) traverser.traverse(target);
        invoker.deleteAcl(ethernetSwitchAcl);
    }
}

