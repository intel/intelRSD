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
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.BindPortActionInvoker;
import com.intel.podm.business.redfish.services.actions.UnbindPortActionInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.logger.Logger;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.Violations.createWithViolations;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Stateless
public class AclPortActionsService {
    @Inject
    private Logger logger;

    @Inject
    private EntityTreeTraverser entityTreeTraverser;

    @Inject
    private BindPortActionInvoker bindPortActionInvoker;

    @Inject
    private UnbindPortActionInvoker unbindPortActionInvoker;

    @Transactional(REQUIRES_NEW)
    public void bindPort(Context aclContext, Context portContext) throws BusinessApiException {
        EthernetSwitchAcl acl = (EthernetSwitchAcl) entityTreeTraverser.tryTraverse(aclContext)
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Ethernet Switch ACL does not exist")));
        EthernetSwitchPort port = (EthernetSwitchPort) entityTreeTraverser.tryTraverse(portContext)
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Ethernet Switch PORT does not exist")));

        validateBindPortRequest(acl, port);
        bindPortActionInvoker.bindPortToAcl(acl, port);
    }

    private void validateBindPortRequest(EthernetSwitchAcl acl, EthernetSwitchPort port) throws RequestValidationException {
        if (!acl.getBindActionAllowableValues().contains(port)) {
            if (acl.getBoundPorts().contains(port)) {
                throw new RequestValidationException(createWithViolations("Port is already bound"));
            } else {
                throw new RequestValidationException(createWithViolations("Port is not bindable"));
            }
        }
    }

    @Transactional(REQUIRES_NEW)
    public void unbindPort(Context aclContext, Context portContext) throws BusinessApiException {
        EthernetSwitchAcl acl = (EthernetSwitchAcl) entityTreeTraverser.tryTraverse(aclContext)
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Ethernet Switch ACL does not exist")));
        EthernetSwitchPort port = (EthernetSwitchPort) entityTreeTraverser.tryTraverse(portContext)
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Ethernet Switch PORT does not exist")));

        validateUnbindPortRequest(acl, port);
        unbindPortActionInvoker.unbindPortFromAcl(acl, port);
    }

    private void validateUnbindPortRequest(EthernetSwitchAcl acl, EthernetSwitchPort port) throws RequestValidationException {
        if (!acl.getBoundPorts().contains(port)) {
            throw new RequestValidationException(createWithViolations("Port is not bound"));
        }
    }
}
