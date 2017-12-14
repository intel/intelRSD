/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.ComputerSystemUpdateInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.redfish.services.ComputerSystemUpdater.validateBootSupport;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeUpdater {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Transactional(REQUIRES_NEW)
    public void updateComposedNode(Context context, RedfishComputerSystem representation) throws BusinessApiException {
        ComputerSystem computerSystem = ((ComposedNode) traverser.traverse(context)).getComputerSystem();
        validate(computerSystem);

        ComputerSystemUpdateDefinition updateDefinition = createComputerSystemUpdateDefinition(representation);
        validateBootSupportForComposedNode(updateDefinition.getBootSourceType(), computerSystem);

        computerSystemUpdateInvoker.updateComputerSystem(computerSystem, updateDefinition);
    }

    private ComputerSystemUpdateDefinition createComputerSystemUpdateDefinition(RedfishComputerSystem representation) {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();

        RedfishComputerSystem.Boot boot = representation.getBoot();
        if (boot != null) {
            computerSystemUpdateDefinition.setBootSourceMode(boot.getBootSourceOverrideMode());
            computerSystemUpdateDefinition.setBootSourceState(boot.getBootSourceOverrideEnabled());
            computerSystemUpdateDefinition.setBootSourceType(boot.getBootSourceOverrideTarget());
        }

        return computerSystemUpdateDefinition;
    }

    private void validate(ComputerSystem computerSystem) throws ResourceStateMismatchException {
        if (!computerSystem.isEnabledAndHealthy()) {
            throw new ResourceStateMismatchException("Computer System should be enabled and healthy in order to invoke actions on it.");
        }
    }

    private void validateBootSupportForComposedNode(BootSourceType type, ComputerSystem computerSystem) throws BusinessApiException {
        validateBootSupport(type, computerSystem);
    }
}
