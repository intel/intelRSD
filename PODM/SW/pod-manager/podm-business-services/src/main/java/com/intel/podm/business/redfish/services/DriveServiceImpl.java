/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ChassisSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.DriveMerger;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class DriveServiceImpl implements ReaderService<DriveDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private ChassisSubResourcesFinder chassisSubResourcesFinder;

    @Inject
    private DriveMerger driveMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(context);

        // Multi-source resources sanity check
        if (chassis.isComplementary()) {
            throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
        }

        return new CollectionDto(DRIVES, getAsIdSet(chassisSubResourcesFinder.getUniqueSubResourcesOfClass(chassis, Drive.class)));
    }

    @Transactional(REQUIRED)
    @Override
    public DriveDto getResource(Context context) throws ContextResolvingException {
        Drive drive = (Drive) multiTraverser.traverse(context);
        DriveDto dto = driveMerger.toDto(drive);
        dto.getActions().getSecureEraseAction().setTarget(singletonContextOf(context, "Actions/Drive.SecureErase"));
        return dto;
    }
}
