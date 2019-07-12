/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.dto.ComposedNodeDto;
import com.intel.rsd.nodecomposer.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.ODataIds;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ComputerSystemResource;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Boot;
import com.intel.rsd.nodecomposer.types.PowerState;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.HashSet;
import java.util.Set;

import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asDriveODataIds;
import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asEthernetInterfaceODataIds;
import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asMemoryODataIds;
import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asProcessorODataIds;
import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asSimpleStorageODataIds;
import static com.intel.rsd.nodecomposer.business.redfish.ODataIdCollections.asVolumeODataIds;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.ATTACH_RESOURCE_ACTION_INFO;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ASSEMBLE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ATTACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_DETACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_FORCE_DELETE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_RESET;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.DETACH_RESOURCE_ACTION_INFO;
import static java.lang.String.format;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeReader {
    private final EntityTreeTraverser traverser;
    private final ComposedNodeDao composedNodeDao;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public ComposedNodeReader(EntityTreeTraverser traverser, ComposedNodeDao composedNodeDao, WebClientBuilder webClientBuilder) {
        this.traverser = traverser;
        this.composedNodeDao = composedNodeDao;
        this.webClientBuilder = webClientBuilder;
    }

    public ComposedNodeCollectionDto getCollection() {
        return new ComposedNodeCollectionDto(composedNodeDao.getAllComposedNodeUris());
    }

    public ComposedNodeDto getResource(ODataId composedNodeODataId) throws ODataIdResolvingException {
        ComposedNode composedNode = traverser.traverseComposedNode(composedNodeODataId);

        Set<ODataId> processors = new HashSet<>();
        ComposedNodeDto composedNodeDto = new ComposedNodeDto();
        fillFromNode(composedNodeDto, composedNode);

        setSystemConnectedValues(composedNode, processors, composedNodeDto);
        setComposedNodeClearFlags(composedNode, composedNodeDto);
        setTaggedValues(composedNode, composedNodeDto);

        return composedNodeDto;
    }

    private void setSystemConnectedValues(ComposedNode composedNode, Set<ODataId> processors, ComposedNodeDto composedNodeDto) {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem != null) {
            fillFromComputerSystem(composedNodeDto, computerSystem);
            processors.addAll(asProcessorODataIds(computerSystem.getProcessors()));
        }
        processors.addAll(asProcessorODataIds(composedNode.getProcessors()));
        composedNodeDto.getLinks().setProcessors(processors);
    }

    private void setComposedNodeClearFlags(ComposedNode composedNode, ComposedNodeDto composedNodeDto) {
        composedNodeDto.setClearTpmOnDelete(composedNode.isClearTpmOnDelete());
        composedNodeDto.setClearOptanePersistentMemoryOnDelete(composedNode.isClearOptanePersistentMemoryOnDelete());
    }

    private void setTaggedValues(ComposedNode composedNode, ComposedNodeDto composedNodeDto) {
        if (!composedNode.getTaggedValues().isEmpty()) {
            ComposedNodeDto.Oem.IntelRackScale intelRackScale = new ComposedNodeDto.Oem.IntelRackScale();
            intelRackScale.setTaggedValues(composedNode.getTaggedValues());
            composedNodeDto.getOem().setIntelRackScale(intelRackScale);
        }
    }

    private void fillFromNode(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        composedNodeDto.setId(composedNode.getComposedNodeId().toString());
        composedNodeDto.setName(composedNode.getName());
        composedNodeDto.setDescription(composedNode.getDescription());
        composedNodeDto.setStatus(composedNode.getStatus());
        composedNodeDto.setComposedNodeState(composedNode.getComposedNodeState());

        fillNodeLinks(composedNodeDto, composedNode);
        fillNodeActions(composedNodeDto, composedNode);
    }

    private void fillNodeLinks(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        ComposedNodeDto.Links links = composedNodeDto.getLinks();
        links.getEthernetInterfaces().addAll(asEthernetInterfaceODataIds(composedNode.getEthernetInterfaces()));

        links.getStorage().addAll(asVolumeODataIds(composedNode.getVolumes()));
        links.getStorage().addAll(asDriveODataIds(composedNode.getLocalDrives()));
        links.getStorage().addAll(asSimpleStorageODataIds(composedNode.getSimpleStorages()));
        links.getStorage().addAll(asDriveODataIds(composedNode.getDrives()));
    }

    private void fillNodeActions(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        ODataId oDataId = composedNode.getUri();
        ComposedNodeDto.Actions actions = composedNodeDto.getActions();

        actions.getResetAction().setTarget(getActionString(oDataId, COMPOSED_NODE_RESET));
        actions.getAssembleAction().setTarget(getActionString(oDataId, COMPOSED_NODE_ASSEMBLE));
        actions.getAttachResourceAction().setTarget(getActionString(oDataId, COMPOSED_NODE_ATTACH_RESOURCE));
        actions.getAttachResourceAction().setActionInfo(getActionString(oDataId, ATTACH_RESOURCE_ACTION_INFO));
        actions.getDetachResourceAction().setTarget(getActionString(oDataId, COMPOSED_NODE_DETACH_RESOURCE));
        actions.getDetachResourceAction().setActionInfo(getActionString(oDataId, DETACH_RESOURCE_ACTION_INFO));
        actions.getForceDeleteAction().setTarget(getActionString(oDataId, COMPOSED_NODE_FORCE_DELETE));
    }

    private String getActionString(ODataId oDataId, String actionString) {
        return oDataId + format("/Actions/%s", actionString);
    }

    private void fillFromComputerSystem(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        fillBoot(composedNodeDto, computerSystem);

        composedNodeDto.getActions().getResetAction().setAllowableResetTypes(computerSystem.getAllowableResetTypes());
        composedNodeDto.setUuid(computerSystem.getUuid());
        composedNodeDto.setPowerState(fetchPowerState(computerSystem));
        composedNodeDto.getLinks().setMemory(asMemoryODataIds(computerSystem.getMemoryModules()));
        composedNodeDto.getLinks().setComputerSystem(ODataIds.toODataId(computerSystem));
    }

    private PowerState fetchPowerState(ComputerSystem computerSystem) {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().build()) {
            try {
                ComputerSystemResource computerSystemResource = (ComputerSystemResource) webClient.get(computerSystem.getUri().toUri());
                return computerSystemResource.getPowerState().orElse(null);
            } catch (WebClientRequestException e) {
                log.warn("Could not fetch Computer System {} power state, returning cached one. Error: {}", computerSystem.getUri(), e);
                return computerSystem.getPowerState();
            }
        }
    }

    private void fillBoot(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        ComposedNodeDto.BootDto bootDto = composedNodeDto.getBoot();
        Boot boot = computerSystem.getBoot();
        if (boot == null) {
            return;
        }

        bootDto.setBootSourceOverrideEnabled(boot.getBootSourceOverrideEnabled());
        bootDto.setBootSourceOverrideTarget(boot.getBootSourceOverrideTarget());
        bootDto.setBootSourceOverrideTargetAllowableValues(boot.getBootSourceOverrideTargetAllowableValues());
        bootDto.setBootSourceOverrideMode(boot.getBootSourceOverrideMode());
        bootDto.setBootSourceOverrideModeAllowableValues(boot.getBootSourceOverrideModeAllowableValues());
    }
}
