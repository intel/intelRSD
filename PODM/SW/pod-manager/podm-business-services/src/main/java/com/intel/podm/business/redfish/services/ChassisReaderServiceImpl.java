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
import com.intel.podm.business.dto.ChassisDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ChassisMerger;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.CHASSIS;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class ChassisReaderServiceImpl implements ReaderService<ChassisDto> {
    @Inject
    private ChassisDao chassisDao;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ChassisMerger chassisMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        return new CollectionDto(CHASSIS, chassisDao.findAllChassisFromPrimaryDataSource());
    }

    @Transactional(REQUIRED)
    @Override
    public ChassisDto getResource(Context context) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(context);

        // Multi-source resources sanity check
        if (chassis.isComplementary()) {
            throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
        }

        ChassisDto dto = chassisMerger.toDto(chassis);
        if (chassis.getThermal() != null) {
            dto.setThermal(singletonContextOf(context, THERMAL_RESOURCE_NAME));
        }
        if (chassis.getPower() != null) {
            dto.setPower(singletonContextOf(context, POWER_RESOURCE_NAME));
        }
        return dto;
    }
}
