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

package com.intel.podm.allocation.validation;

import com.intel.podm.allocation.RequestValidationException;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Set;

import static java.util.Collections.emptySet;

@Dependent
public class ComputerSystemAllocationValidator {
    @Inject
    private ComputerSystemCollector computerSystemCollector;

    @Inject
    private EthernetInterfacesValidator ethernetInterfacesValidator;

    public Violations validate(RequestedNode requestedNode) {
        Violations violations = new Violations();
        violations.addAll(validateComputerSystemsAbilities(requestedNode));

        if (requestedNode.getName() == null) {
            violations.addViolation("Name of Composed Node cannot be null");
        }

        violations.addAll(ethernetInterfacesValidator.validate(requestedNode.getEthernetInterfaces()));

        return violations;
    }

    private Violations validateComputerSystemsAbilities(RequestedNode requestedNode) {
        Violations violations = new Violations();

        Set<ComputerSystem> computerSystems = emptySet();
        try {
            computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(requestedNode);
        } catch (RequestValidationException e) {
            violations.addAll(e.getViolations());
        }

        if (!canBeRealizedBySingleComputerSystem(computerSystems)) {
            violations.addViolation("Allocation of assets on multiple computer systems is not supported");
        }

        try {
            computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
        } catch (RequestValidationException e) {
            violations.addAll(e.getViolations());
        }

        return violations;
    }

    private boolean canBeRealizedBySingleComputerSystem(Set<ComputerSystem> computerSystems) {
        return computerSystems.size() <= 1;
    }
}
