/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.ComputerSystemUpdateInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.common.types.redfish.RedfishComputerSystem.TrustedModule;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemUpdater {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Transactional(REQUIRES_NEW)
    public void updateComputerSystem(Context context, RedfishComputerSystem representation) throws BusinessApiException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(context);
        validate(computerSystem);

        ComputerSystemUpdateDefinition updateDefinition = createComputerSystemUpdateDefinition(representation);
        validateBootSupport(updateDefinition.getBootSourceType(), computerSystem);
        validateTrustedModulesSupport(updateDefinition.getTrustedModules(), computerSystem);

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

        computerSystemUpdateDefinition.setUserModeEnabled(representation.getUserModeEnabled());
        computerSystemUpdateDefinition.setAssetTag(representation.getAssetTag());
        computerSystemUpdateDefinition.setTrustedModules(representation.getTrustedModules());

        return computerSystemUpdateDefinition;
    }

    private void validate(ComputerSystem computerSystem) throws ResourceStateMismatchException {
        if (!statusOf(computerSystem).isEnabled().isHealthy().verify()) {
            throw new ResourceStateMismatchException("Computer System should be enabled and healthy in order to invoke actions on it.");
        }
    }

    private void validateTrustedModulesSupport(Set<TrustedModule> trustedModules, ComputerSystem computerSystem) throws RequestValidationException {
        if (nullOrEmpty(trustedModules)) {
            return;
        }

        if (nullOrEmpty(computerSystem.getTrustedModules())) {
            throw new RequestValidationException(createWithViolations("Provided Computer System does not support TPM technology."));
        }
    }

    static void validateBootSupport(BootSourceType type, ComputerSystem computerSystem) throws BusinessApiException {
        if (type == null) {
            return;
        }

        Boot boot = computerSystem.getBoot();
        if (boot == null) {
            throw new ResourceStateMismatchException("Provided Computer System does not have Boot property.");
        }

        Collection<BootSourceType> allowableBootSourceTypes = boot.getBootSourceOverrideTargetAllowableValues();
        if (!allowableBootSourceTypes.contains(type)) {
            String violation = format("Provided BootSourceType is invalid. Allowable values: %s", allowableBootSourceTypes);
            throw new RequestValidationException(createWithViolations(violation));
        }
    }
}
