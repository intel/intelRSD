/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.actions.VolumeActionDto;
import com.intel.podm.business.dto.redfish.CapacitySourceDto;
import com.intel.podm.business.dto.redfish.ReplicaInfoDto;
import com.intel.podm.business.dto.redfish.VolumeDto;
import com.intel.podm.business.entities.redfish.CapacitySource;
import com.intel.podm.business.entities.redfish.ReplicaInfo;
import com.intel.podm.business.entities.redfish.Volume;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

import static java.lang.Boolean.TRUE;

@Dependent
public class VolumeDtoMapper extends DtoMapper<Volume, VolumeDto> {
    @Inject
    private CapacitySourcesDtoMapper capacitySourcesDtoMapper;

    @Inject
    private ReplicaInfoDtoMapper replicaInfoDtoMapper;

    VolumeDtoMapper() {
        super(Volume.class, VolumeDto.class);
        this.ignoredProperties("capacitySources", "replicaInfos");
    }

    @Override
    protected void performNotAutomatedMapping(Volume source, VolumeDto target) {
        super.performNotAutomatedMapping(source, target);
        if (Objects.equals(source.getInitializeActionSupported(), TRUE)) {
            target.getActions().setInitializeAction(new VolumeActionDto());
            target.getActions().getInitializeAction().setAllowableValues(source.getAllowableInitializeTypes());
        }
        mapCapacitySources(source, target);
        mapReplicaInfos(source, target);
        mapRackScaleOem(source, target);
    }

    private void mapCapacitySources(Volume source, VolumeDto target) {
        for (CapacitySource capacitySource : source.getCapacitySources()) {
            CapacitySourceDto dto = new CapacitySourceDto();
            capacitySourcesDtoMapper.map(capacitySource, dto);
            target.getCapacitySources().add(dto);
        }
    }

    private void mapReplicaInfos(Volume source, VolumeDto target) {
        for (ReplicaInfo replicaInfo : source.getReplicaInfos()) {
            ReplicaInfoDto dto = new ReplicaInfoDto();
            replicaInfoDtoMapper.map(replicaInfo, dto);
            target.getReplicaInfos().add(dto);
        }
    }

    private void mapRackScaleOem(Volume source, VolumeDto target) {
        VolumeDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setBootable(source.getBootable());
        rackScaleOem.setEraseOnDetach(source.getEraseOnDetach());
        rackScaleOem.setErased(source.getErased());
    }
}
