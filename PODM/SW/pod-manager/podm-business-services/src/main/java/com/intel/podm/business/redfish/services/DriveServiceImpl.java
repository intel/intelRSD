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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.DriveDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ChassisSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.DriveMerger;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVE;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.DRIVE_METRICS_RESOURCE_NAME;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
class DriveServiceImpl implements ReaderService<DriveDto> {
    @Inject
    EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private ChassisSubResourcesFinder chassisSubResourcesFinder;

    @Inject
    private DriveMerger driveMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        switch (context.getType()) {
            case CHASSIS:
                Chassis chassis = (Chassis) traverser.traverse(context);
                if (chassis.isComplementary()) {
                    throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
                }
                return new CollectionDto(DRIVE, getDriveContextsFromChassis(chassis));
            case STORAGE_SERVICE:
                StorageService storageService = (StorageService) traverser.traverse(context);
                return new CollectionDto(DRIVE, storageService.getDrives().stream().map(Contexts::toContext).sorted().collect(toList()));
            default:
                throw new ContextResolvingException("Unsupported parent context of Drive Collection", context, null);
        }
    }

    @Transactional(REQUIRED)
    @Override
    public DriveDto getResource(Context context) throws ContextResolvingException {
        Drive drive = (Drive) multiTraverser.traverse(context);
        DriveDto dto = driveMerger.toDto(drive);
        dto.getActions().getSecureEraseAction().setTarget(singletonContextOf(context, "Actions/Drive.SecureErase"));

        if (drive.getMetrics() != null) {
            dto.getOem().getRackScaleOem().setMetrics(singletonContextOf(context, DRIVE_METRICS_RESOURCE_NAME));
        }

        return dto;
    }

    private List<Context> getDriveContextsFromChassis(Chassis chassis) {
        return chassisSubResourcesFinder.getUniqueSubResourcesOfClass(chassis, Drive.class).stream()
            .map(Contexts::toContext)
            .sorted()
            .collect(toList());
    }
}
