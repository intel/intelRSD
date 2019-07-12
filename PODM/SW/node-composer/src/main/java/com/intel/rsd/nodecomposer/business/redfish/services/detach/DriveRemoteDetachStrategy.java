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
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.DriveResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import lombok.SneakyThrows;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component("DRIVE")
public class DriveRemoteDetachStrategy implements RemoteDetachStrategy<Drive> {

    private final SecureEraseInvoker<Drive, DriveResource> secureEraseInvoker;

    private final RemoteDetachStrategyAdapter<Drive> delegate;

    @Autowired
    public DriveRemoteDetachStrategy(SecureEraseInvoker<Drive, DriveResource> secureEraseInvoker, RemoteDetachStrategyAdapter<Drive> delegate) {
        this.secureEraseInvoker = secureEraseInvoker;
        this.delegate = delegate;
    }

    @Override
    public void detachOnRemoteFabric(Drive assetToBeDetached, AttachDetachOperationContext context) throws EntityOperationException {
        delegate.detachOnRemoteFabric(assetToBeDetached, context);
    }

    @SneakyThrows
    @Override
    public void onAfterRemoteDetach(Drive drive) {
        secureEraseInvoker.updateErased(drive, false);
    }

}
