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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.SecureEraseInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ProcessorResource;
import com.intel.rsd.nodecomposer.mappers.redfish.ProcessorMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.PROCESSOR;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component(PROCESSOR)
@Scope(SCOPE_SINGLETON)
public class ProcessorDetacher extends Detacher<Processor> {
    private final ProcessorHelper processorHelper;
    private final ProcessorMapper processorMapper;
    private final SecureEraseInvoker<Processor, ProcessorResource> secureEraseInvoker;

    public ProcessorDetacher(ProcessorHelper processorHelper, ProcessorMapper processorMapper,
                             SecureEraseInvoker<Processor, ProcessorResource> secureEraseInvoker) {
        this.processorHelper = processorHelper;
        this.processorMapper = processorMapper;
        this.secureEraseInvoker = secureEraseInvoker;
    }

    @Override
    protected void validate(ComposedNode composedNode, Processor processor) throws RequestValidationException {
        if (!composedNode.getProcessors().contains(processor)) {
            throw new RequestValidationException(createWithViolations("Provided processor is not attached to this Node."));
        }
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Processor processor) {
        return processorHelper.retrieveFabricFromProcessor(processor);
    }

    @Override
    protected void performResourceSpecificActions(ComposedNode composedNode, Processor processor) throws EntityOperationException {
        secureEraseInvoker.updateErased(processor, false, processorMapper);
    }
}
