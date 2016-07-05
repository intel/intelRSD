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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.LogicalDriveDto;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.LogicalDriveService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.LOGICAL_DRIVES;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class LogicalDriveServiceImpl implements LogicalDriveService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getLogicalDriveCollection(Context context) throws EntityNotFoundException {
        StorageService storageService = (StorageService) traverser.traverse(context);
        return new CollectionDto(LOGICAL_DRIVES, Contexts.getAsIdList(storageService.getLogicalDrives()));
    }

    @Override
    public LogicalDriveDto getLogicalDrive(Context context) throws EntityNotFoundException {
        LogicalDrive drive = (LogicalDrive) traverser.traverse(context);
        return LogicalDriveDto.newBuilder()
                .id(drive.getId())
                .description(drive.getDescription())
                .name(drive.getName())
                .status(drive.getStatus())
                .type(drive.getType())
                .mode(drive.getMode())
                .writeProtected(drive.getWriteProtected())
                .snapshot(drive.getSnapshot())
                .capacityGib(drive.getCapacityGib())
                .image(drive.getImage())
                .bootable(drive.getBootable())
                .context(context)
                .physicalDrives(Contexts.asPhysicalDriveContexts(drive.getPhysicalDrives()))
                .masterDrive(Contexts.toContext(drive.getMasterDrive()))
                .logicalDrives(Contexts.asLogicalDriveContexts(drive.getLogicalDrives()))
                .usedBy(Contexts.asLogicalDriveContexts(drive.getUsedBy()))
                .targets(Contexts.asRemoteTargetContexts(drive.getRemoteTargets()))
                .build();
    }
}
