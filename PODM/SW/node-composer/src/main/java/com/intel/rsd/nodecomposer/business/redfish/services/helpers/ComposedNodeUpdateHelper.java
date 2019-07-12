/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ComputerSystemUpdateInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Boot;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.ComposedNodePartialRepresentation;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeUpdateHelper {
    private final EntityTreeTraverser traverser;
    private final ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Autowired
    public ComposedNodeUpdateHelper(EntityTreeTraverser traverser, ComputerSystemUpdateInvoker computerSystemUpdateInvoker) {
        this.traverser = traverser;
        this.computerSystemUpdateInvoker = computerSystemUpdateInvoker;
    }

    @Transactional(value = REQUIRES_NEW, rollbackOn = BusinessApiException.class)
    public void updateComposedNode(ODataId composedNodeODataId, ComposedNodePartialRepresentation representation) throws BusinessApiException {
        ComposedNode composedNode = traverser.traverseComposedNode(composedNodeODataId);
        ComputerSystem computerSystem = composedNode.getComputerSystem();

        ifNeededUpdateBoot(representation, computerSystem);
        ifNeededUpdateTaggedValues(representation, composedNode);

        ofNullable(representation.getClearTpmOnDelete()).ifPresent(composedNode::setClearTpmOnDelete);
        ofNullable(representation.getClearOptanePersistentMemoryOnDelete()).ifPresent(composedNode::setClearOptanePersistentMemoryOnDelete);
    }

    private void ifNeededUpdateTaggedValues(ComposedNodePartialRepresentation representation, ComposedNode composedNode) {
        if (representationContainsTaggedValues(representation)) {
            composedNode.setTaggedValues(representation.getOem().getIntelRackScale().getTaggedValues());
        }
    }

    private void ifNeededUpdateBoot(ComposedNodePartialRepresentation representation, ComputerSystem computerSystem) throws BusinessApiException {
        validate(computerSystem);

        if (representation.getBoot() != null) {
            ComputerSystemUpdateDefinition updateDefinition = createComputerSystemUpdateDefinition(representation);
            validateBootSupportForComposedNode(updateDefinition.getBootSourceType(), computerSystem);

            computerSystemUpdateInvoker.updateComputerSystem(computerSystem, updateDefinition);
        }
    }

    private boolean representationContainsTaggedValues(ComposedNodePartialRepresentation representation) {
        ComposedNodePartialRepresentation.Oem oem = representation.getOem();
        return oem != null && oem.getIntelRackScale() != null && oem.getIntelRackScale().getTaggedValues() != null;
    }

    private ComputerSystemUpdateDefinition createComputerSystemUpdateDefinition(ComposedNodePartialRepresentation representation) {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();

        ComposedNodePartialRepresentation.Boot boot = representation.getBoot();
        if (boot != null) {
            computerSystemUpdateDefinition.setBootSourceMode(boot.getBootSourceOverrideMode());
            computerSystemUpdateDefinition.setBootSourceState(boot.getBootSourceOverrideEnabled());
            computerSystemUpdateDefinition.setBootSourceType(boot.getBootSourceOverrideTarget());
        }

        return computerSystemUpdateDefinition;
    }

    private void validate(ComputerSystem computerSystem) throws ResourceStateMismatchException {
        if (computerSystem == null || !statusOf(computerSystem).isEnabled().isHealthy().verify()) {
            throw new ResourceStateMismatchException("Computer System should be enabled and healthy in order to invoke actions on it.");
        }
    }

    private void validateBootSupportForComposedNode(String type, ComputerSystem computerSystem) throws BusinessApiException {
        validateBootSupport(type, computerSystem);
    }

    private void validateBootSupport(String target, ComputerSystem computerSystem) throws BusinessApiException {
        if (target == null) {
            return;
        }

        Boot boot = computerSystem.getBoot();
        if (boot == null) {
            throw new ResourceStateMismatchException("Provided Computer System does not have Boot property.");
        }

        Collection<String> allowableBootOverrideTargets = boot.getBootSourceOverrideTargetAllowableValues();
        if (!allowableBootOverrideTargets.contains(target)) {
            String violation = format("Provided BootSourceOverrideTarget is invalid. Allowable values: %s", allowableBootOverrideTargets);
            throw new RequestValidationException(createWithViolations(violation));
        }
    }
}
