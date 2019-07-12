/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ActionInfoDto;
import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ParameterDto;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ParametersDtoBuilder.asResourceParameterDto;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper.FPGA_PROCESSOR_PROTOCOLS;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.DETACH_RESOURCE_ACTION_INFO;
import static java.util.stream.Collectors.toSet;

@Component
public class DetachResourceInfoService {
    private final VolumeHelper volumeHelper;
    private final ProcessorHelper processorHelper;

    @Autowired
    public DetachResourceInfoService(VolumeHelper volumeHelper, ProcessorHelper processorHelper) {
        this.volumeHelper = volumeHelper;
        this.processorHelper = processorHelper;
    }

    public ActionInfoDto getActionInfo(ComposedNode composedNode) {
        ActionInfoDto actionInfoDto = new ActionInfoDto();
        actionInfoDto.setId(DETACH_RESOURCE_ACTION_INFO);
        actionInfoDto.setName("Detach Resource ActionInfo");
        List<ParameterDto> parameterList = new ArrayList<>();
        parameterList.add(asResourceParameterDto(getResourcesAllowableForDetach(composedNode)));
        actionInfoDto.addParameters(parameterList);

        return actionInfoDto;
    }

    private Set<ODataId> getResourcesAllowableForDetach(ComposedNode composedNode) {
        Set<Volume> volumes = composedNode.getVolumes().stream().filter(excludeIscsiPortVolume()).collect(toSet());
        Set<Endpoint> endpoints = composedNode.getEndpoints().stream().filter(excludeIscsiPortEndpoint()).collect(toSet());
        Set<Processor> processors = composedNode.getProcessors().stream().filter(excludeNonFabricProcessors()).collect(toSet());

        Set<ODataId> resources = new HashSet<>(getODataId(composedNode.getDrives()));
        resources.addAll(getODataId(processors));
        resources.addAll(getODataId(volumes));
        resources.addAll(getODataId(endpoints));

        return resources;
    }

    private Predicate<Processor> excludeNonFabricProcessors() {
        return processor -> FPGA_PROCESSOR_PROTOCOLS.contains(processorHelper.retrieveProtocolFromProcessor(processor));
    }

    private Set<ODataId> getODataId(Set<? extends DiscoverableEntity> discoverableEntities) {
        return discoverableEntities.stream().map(DiscoverableEntity::getUri)
            .collect(toSet());
    }

    private Predicate<Endpoint> excludeIscsiPortEndpoint() {
        return endpoint -> !ISCSI.equals(endpoint.getProtocol());
    }

    private Predicate<Volume> excludeIscsiPortVolume() {
        return volume -> !ISCSI.equals(volumeHelper.retrieveProtocolFromVolume(volume));
    }
}
