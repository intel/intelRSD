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
import com.intel.podm.business.dto.ComposedNodeDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetInterfaceContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asMemoryContexts;
import static com.intel.podm.business.redfish.ContextCollections.asProcessorContexts;
import static com.intel.podm.business.redfish.ContextCollections.asSimpleStorageContexts;
import static com.intel.podm.business.redfish.ContextCollections.asVolumeContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.actions.ActionInfoNames.ATTACH_RESOURCE_ACTION_INFO;
import static com.intel.podm.common.types.actions.ActionInfoNames.DETACH_RESOURCE_ACTION_INFO;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class ComposedNodeReader implements ReaderService<ComposedNodeDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ComposedNodeDao composedNodeDao;

    @Inject
    private ChassisDao chassisDao;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) {
        List<Context> nodeContexts = composedNodeDao.getAllComposedNodeIds().stream().map(id -> contextOf(id, ContextType.COMPOSED_NODE)).sorted()
            .collect(toList());
        return new ComposedNodeCollectionDto(CollectionDto.Type.COMPOSED_NODE, nodeContexts);
    }

    @Transactional(REQUIRED)
    @Override
    public ComposedNodeDto getResource(Context composedNodeContext) throws ContextResolvingException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);

        ComposedNodeDto composedNodeDto = new ComposedNodeDto();
        fillFromNode(composedNodeDto, composedNode);

        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem != null) {
            fillFromComputerSystem(composedNodeDto, computerSystem);
        }

        composedNodeDto.setClearTpmOnDelete(composedNode.getClearTpmOnDelete());

        return composedNodeDto;
    }

    private void fillFromNode(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        composedNodeDto.setId(composedNode.getId().toString());
        composedNodeDto.setName(composedNode.getName());
        composedNodeDto.setDescription(composedNode.getDescription());
        composedNodeDto.setStatus(composedNode.getStatus());
        composedNodeDto.setComposedNodeState(composedNode.getComposedNodeState());

        fillNodeLinks(composedNodeDto, composedNode);
        fillNodeActions(composedNodeDto, composedNode);
    }

    private void fillNodeLinks(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        ComposedNodeDto.Links links = composedNodeDto.getLinks();
        links.getEthernetInterfaces().addAll(asEthernetInterfaceContexts(composedNode.getEthernetInterfaces()));
        links.getManagedBy().addAll(asManagerContexts(single(chassisDao.getAllByChassisType(POD)).getManagers()));

        links.getStorage().addAll(asVolumeContexts(composedNode.getVolumes()));
        links.getStorage().addAll(asDriveContexts(composedNode.getLocalDrives()));
        links.getStorage().addAll(asSimpleStorageContexts(composedNode.getSimpleStorages()));
        links.getStorage().addAll(asDriveContexts(composedNode.getDrives()));
    }

    private void fillNodeActions(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        Context context = contextOf(composedNode.getId(), ContextType.COMPOSED_NODE);
        ComposedNodeDto.Actions actions = composedNodeDto.getActions();
        actions.getResetAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.Reset"));
        actions.getAssembleAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.Assemble"));
        actions.getAttachResourceAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.AttachResource"));
        actions.getAttachResourceAction().setActionInfo(singletonContextOf(context, format("Actions/%s", ATTACH_RESOURCE_ACTION_INFO)));
        actions.getDetachResourceAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.DetachResource"));
        actions.getDetachResourceAction().setActionInfo(singletonContextOf(context, format("Actions/%s", DETACH_RESOURCE_ACTION_INFO)));

    }

    private void fillFromComputerSystem(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        fillBoot(composedNodeDto, computerSystem);

        composedNodeDto.getActions().getResetAction().setAllowableResetTypes(computerSystem.getAllowableResetTypes());
        composedNodeDto.setUuid(computerSystem.getUuid());
        composedNodeDto.setPowerState(computerSystem.getPowerState());
        composedNodeDto.getLinks().setProcessors(asProcessorContexts(computerSystem.getProcessors()));
        composedNodeDto.getLinks().setMemory(asMemoryContexts(computerSystem.getMemoryModules()));
        composedNodeDto.getLinks().setComputerSystem(toContext(computerSystem));
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
