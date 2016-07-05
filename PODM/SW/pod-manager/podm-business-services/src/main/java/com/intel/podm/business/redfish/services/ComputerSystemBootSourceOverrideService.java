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
import com.intel.podm.actions.BootSourceOverrideInvoker;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.properties.Boot;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ComputerSystemBootSourceOverrideService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private BootSourceOverrideInvoker bootSourceOverrideInvoker;

    public void overrideBootSource(Context context, BootSourceType type, BootSourceState state)
            throws EntityNotFoundException, ActionException, IllegalArgumentException, IllegalStateException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(context);
        overrideBootSource(computerSystem, type, state);
    }

    public void overrideBootSource(ComputerSystem computerSystem, BootSourceType type, BootSourceState state)
            throws EntityNotFoundException, ActionException, IllegalStateException {
        validateParameters(type, computerSystem);
        bootSourceOverrideInvoker.overrideBootSource(computerSystem, type, state);
    }

    private void validateParameters(BootSourceType type, ComputerSystem computerSystem) throws IllegalArgumentException, IllegalStateException {
        Boot boot = computerSystem.getBoot();

        if (boot == null) {
            throw new IllegalStateException("Provided Computer system does not have Boot property");
        }

        List<BootSourceType> allowableBootSourceTypes = boot.getBootSourceOverrideTargetAllowableValues();
        if (!allowableBootSourceTypes.contains(type)) {
            throw new IllegalArgumentException(format("Provided BootSourceType should be in allowable BootSourceType's %s", allowableBootSourceTypes));
        }

        if (!computerSystem.isEnabledAndHealthy()) {
            throw new IllegalStateException("Computer system should be enabled and healthy in order to invoke actions on it.");
        }
    }
}
