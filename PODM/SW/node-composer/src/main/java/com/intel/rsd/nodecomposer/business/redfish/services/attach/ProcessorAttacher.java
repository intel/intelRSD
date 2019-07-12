/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachActionValidator;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointFinder;
import com.intel.rsd.nodecomposer.persistence.dao.ProcessorDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Set;

import static com.intel.rsd.nodecomposer.business.redfish.services.attach.Attacher.ATTACHER_BEAN_PREFIX;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.PROCESSOR;
import static com.intel.rsd.nodecomposer.types.ProcessorType.FPGA;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;

@SuppressWarnings("checkstyle:ClassFanOutComplexity")
@Component(ATTACHER_BEAN_PREFIX + PROCESSOR)
public class ProcessorAttacher extends Attacher<Processor> {

    private final ProcessorDao processorDao;
    private final ProcessorHelper processorHelper;

    @Autowired
    public ProcessorAttacher(AttachActionValidator<Processor> attachActionValidator, TargetEndpointFinder targetEndpointFinder,
                             RemoteAttacher remoteAttacher, LocalAttacher localAttacher,
                             ProcessorDao processorDao, ProcessorHelper processorHelper) {

        super(attachActionValidator, targetEndpointFinder, remoteAttacher, localAttacher);
        this.processorDao = processorDao;
        this.processorHelper = processorHelper;
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Processor processor) {
        return processorHelper.retrieveFabricFromProcessor(processor);
    }

    @Override
    protected Protocol retrieveAssetProtocol(Processor processor) {
        return processorHelper.retrieveProtocolFromProcessor(processor);
    }

    @Override
    public Violations getAssetSpecificViolations(ComposedNode composedNode, Processor processor) {
        Violations violations = new Violations();

        addNotFpgaProcessorViolation(processor, violations);
        addProcessorNotAchievableViolation(composedNode, processor, violations);
        addProcessorEndpointsAttachedViolation(processor, violations);

        return violations;
    }

    private void addProcessorEndpointsAttachedViolation(Processor processor, Violations violations) {
        if (!processor.getEndpoints().isEmpty() && processor.getEndpoints().stream().noneMatch(Endpoint::isAttachable)) {
            violations.addViolation("All endpoints attached to selected fpga processor are in use.");
        }
    }

    private void addProcessorNotAchievableViolation(ComposedNode composedNode, Processor processor, Violations violations) {
        if (!processorHelper.processorProtocolEquals(FPGA_OVER_FABRICS, processor)) {
            Set<Processor> achievableProcessors = processorDao.getAchievablePcieProcessors(composedNode.getComputerSystem());
            if (!achievableProcessors.contains(processor)) {
                violations.addViolation("Selected Processor is not achievable for this node!");
            }
        }
    }

    private void addNotFpgaProcessorViolation(Processor processor, Violations violations) {
        if (!FPGA.equals(processor.getProcessorType())) {
            violations.addViolation("Only processor of type FPGA are allowed to attach.");
        }
    }
}
