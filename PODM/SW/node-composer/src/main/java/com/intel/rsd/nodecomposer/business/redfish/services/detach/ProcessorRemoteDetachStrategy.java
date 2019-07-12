/*
 * Copyright (c) 2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.business.redfish.services.actions.SecureEraseInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ProcessorResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class ProcessorRemoteDetachStrategy implements RemoteDetachStrategy<Processor> {

    private final SecureEraseInvoker<Processor, ProcessorResource> secureEraseInvoker;
    private final RemoteDetachStrategyAdapter<Processor> delegate;

    @Autowired
    public ProcessorRemoteDetachStrategy(SecureEraseInvoker<Processor, ProcessorResource> secureEraseInvoker, RemoteDetachStrategyAdapter<Processor> delegate) {
        this.secureEraseInvoker = secureEraseInvoker;
        this.delegate = delegate;
    }

    @Override
    public void detachOnRemoteFabric(Processor assetToBeDetached, AttachDetachOperationContext context) throws EntityOperationException {
        delegate.detachOnRemoteFabric(assetToBeDetached, context);
    }

    @Override
    public void onAfterRemoteDetach(Processor processor) throws EntityOperationException {
        secureEraseInvoker.updateErased(processor, false);
    }
}
