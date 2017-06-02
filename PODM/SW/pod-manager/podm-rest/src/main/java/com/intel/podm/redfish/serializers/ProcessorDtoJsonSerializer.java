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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.ProcessorDto;
import com.intel.podm.business.dto.redfish.attributes.ProcessorIdDto;
import com.intel.podm.redfish.json.templates.ProcessorJson;
import com.intel.podm.redfish.json.templates.ProcessorJson.ProcessorIdJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class ProcessorDtoJsonSerializer extends BaseDtoJsonSerializer<ProcessorDto> {
    protected ProcessorDtoJsonSerializer() {
        super(ProcessorDto.class);
    }

    @Override
    protected ProcessorJson translate(ProcessorDto dto) {
        ProcessorJson processorJson = new ProcessorJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        processorJson.oDataId = oDataId;
        processorJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        processorJson.id = dto.getId();
        processorJson.name = dto.getName();
        processorJson.description = dto.getDescription();
        processorJson.model = dto.getModel();
        processorJson.socket = dto.getSocket();
        processorJson.status = dto.getStatus();
        processorJson.totalCores = dto.getTotalCores();
        processorJson.maxSpeedMHz = dto.getMaxSpeedMhz();
        processorJson.totalThreads = dto.getTotalThreads();
        processorJson.manufacturer = dto.getManufacturer();
        processorJson.processorType = dto.getProcessorType();
        processorJson.instructionSet = dto.getInstructionSet();
        processorJson.processorArchitecture = dto.getProcessorArchitecture();
        fillProcessorId(processorJson.processorId, dto.getProcessorId());
        fillOem(dto, processorJson);
        return processorJson;
    }

    private void fillProcessorId(ProcessorIdJson processorIdJson, ProcessorIdDto processorId) {
        if (processorId == null) {
            return;
        }

        processorIdJson.step = processorId.getStep();
        processorIdJson.vendorId = processorId.getVendorId();
        processorIdJson.microcodeInfo = processorId.getMicrocodeInfo();
        processorIdJson.effectiveModel = processorId.getEffectiveModel();
        processorIdJson.effectiveFamily = processorId.getEffectiveFamily();
        processorIdJson.identificationRegisters = processorId.getIdentificationRegisters();
    }

    private void fillOem(ProcessorDto dto, ProcessorJson processorJson) {
        processorJson.oem.rackScaleOem.brand = dto.getBrand();
        processorJson.oem.rackScaleOem.odataType = "#Intel.Oem.Processor";
        processorJson.oem.rackScaleOem.capabilities.addAll(dto.getCapabilities());
    }

}
