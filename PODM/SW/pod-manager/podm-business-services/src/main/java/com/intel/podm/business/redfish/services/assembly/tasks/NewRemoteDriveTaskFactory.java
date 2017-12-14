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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class NewRemoteDriveTaskFactory {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private GenericDao genericDao;

    public NewRemoteDriveAssemblyTask create(Id logicalVolumeGroupId, URI masterDriveSourceUri, RequestedNode.RemoteDrive drive) {
        NewRemoteDriveAssemblyTask newRemoteDriveAssemblyTask = beanFactory.create(NewRemoteDriveAssemblyTask.class);

        LogicalDrive logicalVolumeGroup = genericDao.find(LogicalDrive.class, logicalVolumeGroupId);

        ExternalService service = logicalVolumeGroup.getService();
        if (service == null) {
            throw new IllegalStateException("ExternalService should be associated with this asset");
        }

        newRemoteDriveAssemblyTask.setRssServiceUri(service.getBaseUri());
        newRemoteDriveAssemblyTask.setRssServiceUuid(service.getUuid());
        newRemoteDriveAssemblyTask.setStorageServiceSourceUri(logicalVolumeGroup.getStorageService().getSourceUri());
        newRemoteDriveAssemblyTask.setLogicalVolumeGroupSourceUri(logicalVolumeGroup.getSourceUri());
        newRemoteDriveAssemblyTask.setMasterDriveSourceUri(masterDriveSourceUri);
        newRemoteDriveAssemblyTask.setRequestedRemoteDrive(drive);

        return newRemoteDriveAssemblyTask;
    }
}
