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

import com.intel.podm.business.dto.ProcessorDto;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.embeddables.Fpga;
import com.intel.podm.business.entities.redfish.embeddables.OnPackageMemory;

import javax.enterprise.context.Dependent;
import java.util.List;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static com.intel.podm.common.utils.StringRepresentation.toMap;

@Dependent
class ProcessorDtoMapper extends DtoMapper<Processor, ProcessorDto> {
    ProcessorDtoMapper() {
        super(Processor.class, ProcessorDto.class);
        this.ignoredProperties("processorMetrics", "oem", "status");
    }

    @Override
    protected void performNotAutomatedMapping(Processor source, ProcessorDto target) {
        super.performNotAutomatedMapping(source, target);

        ProcessorDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setBrand(source.getBrand());
        rackScaleOem.setCapabilities(source.getCapabilities());
        rackScaleOem.setExtendedIdentificationRegisters(
                source.getExtendedIdentificationRegisters() != null
                        ? toMap(source.getExtendedIdentificationRegisters()) : null
        );
        rackScaleOem.setOnPackageMemory(asPackageMemory(source.getOnPackageMemory()));
        rackScaleOem.setThermalDesignPowerWatt(source.getThermalDesignPowerWatt());
        rackScaleOem.setFpga(asFpga(source.getFpga()));
        if (!source.isComplementary()) {
            ofNullable(source.getStatus()).ifPresent(target::setStatus);
        }
    }

    private List<ProcessorDto.OemProcessorMemory> asPackageMemory(List<OnPackageMemory> onPackageMemory) {
        return onPackageMemory.stream().map(memory -> {
            ProcessorDto.OemProcessorMemory dto = new ProcessorDto.OemProcessorMemory();
            dto.setCapacityMb(memory.getCapacityMb());
            dto.setSpeedMhz(memory.getSpeedMhz());
            dto.setType(memory.getType());
            return dto;
        }).collect(toList());
    }

    private ProcessorDto.OemFpga asFpga(Fpga fpga) {
        ProcessorDto.OemFpga dto = new ProcessorDto.OemFpga();
        if (fpga != null) {
            dto.setBitStreamVersion(fpga.getBitStreamVersion());
            dto.setHssiConfiguration(fpga.getHssiConfiguration());
            dto.setHssiSideband(fpga.getHssiSideband());
            dto.setReconfigurationSlots(fpga.getReconfigurationSlots());
            dto.setType(fpga.getType());
        }
        return dto;
    }
}
