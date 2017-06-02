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

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ComputerSystemUpdateInvoker;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.InvalidPayloadException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;

import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemUpdateService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Transactional(REQUIRES_NEW)
    public void updateComputerSystem(Context context, RedfishComputerSystem representation) throws BusinessApiException {

        ComputerSystemUpdateDefinition computerSystemUpdateDefinition =
            createComputerSystemUpdateDefinition(representation);

        ComputerSystem computerSystem = traverse(context);
        validate(computerSystem);
        validateTypeParameter(computerSystemUpdateDefinition.getBootSourceType(), computerSystem);
        try {
            computerSystemUpdateInvoker.updateComputerSystem(computerSystem, computerSystemUpdateDefinition);
        } catch (ActionException e) {
            throw new EntityOperationException(e.getMessage(), e);
        }
    }

    private ComputerSystemUpdateDefinition createComputerSystemUpdateDefinition(RedfishComputerSystem representation) {
        RedfishComputerSystem.Boot boot = representation.getBoot();
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition =
            new ComputerSystemUpdateDefinition();

        computerSystemUpdateDefinition.setAssetTag(representation.getAssetTag());
        if (boot != null) {
            computerSystemUpdateDefinition.setBootSourceMode(boot.getBootSourceOverrideMode());
            computerSystemUpdateDefinition.setBootSourceState(boot.getBootSourceOverrideEnabled());
            computerSystemUpdateDefinition.setBootSourceType(boot.getBootSourceOverrideTarget());
        }

        return computerSystemUpdateDefinition;
    }

    private ComputerSystem traverse(Context context) throws ContextResolvingException {
        switch (context.getType()) {
            case COMPUTER_SYSTEM:
                return (ComputerSystem) traverser.traverse(context);
            case COMPOSED_NODE:
                return ((ComposedNode) traverser.traverse(context)).getComputerSystem();
            default:
                throw new ContextResolvingException(context);
        }
    }

    private void validate(ComputerSystem computerSystem) throws ResourceStateMismatchException {
        if (!computerSystem.isEnabledAndHealthy()) {
            throw new ResourceStateMismatchException("Computer System should be enabled and healthy in order to invoke actions on it.");
        }
    }

    private void validateTypeParameter(BootSourceType type, ComputerSystem computerSystem) throws ResourceStateMismatchException, InvalidPayloadException {
        if (type == null) {
            return;
        }

        Boot boot = computerSystem.getBoot();
        if (boot == null) {
            throw new ResourceStateMismatchException("Provided Computer System does not have Boot property.");
        }

        Collection<BootSourceType> allowableBootSourceTypes = boot.getBootSourceOverrideTargetAllowableValues();
        if (!allowableBootSourceTypes.contains(type)) {
            throw new InvalidPayloadException(format("Provided BootSourceType should be in allowable BootSourceType's %s", allowableBootSourceTypes));
        }
    }
}
