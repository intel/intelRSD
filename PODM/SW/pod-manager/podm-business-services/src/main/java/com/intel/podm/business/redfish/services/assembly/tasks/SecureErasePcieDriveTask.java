/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.redfish.services.actions.PcieDriveActionsInvoker;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class SecureErasePcieDriveTask extends NodeTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private PcieDriveActionsInvoker pcieDriveActionsInvoker;

    @Inject
    private Logger logger;

    private Id pcieDriveId;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        Drive drive = genericDao.find(Drive.class, pcieDriveId);
        try {
            pcieDriveActionsInvoker.secureErase(drive);
        } catch (EntityOperationException e) {
            logger.e("SecureErase action failed: " + e.getMessage(), e);
        }
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return genericDao.find(Drive.class, pcieDriveId).getService().getUuid();
    }

    public SecureErasePcieDriveTask setPcieDriveId(Id pcieDriveId) {
        this.pcieDriveId = pcieDriveId;
        return this;
    }
}
