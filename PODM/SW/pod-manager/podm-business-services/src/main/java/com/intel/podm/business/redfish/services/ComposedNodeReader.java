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
import com.intel.podm.business.dto.ComposedNodeDto;
import com.intel.podm.business.dto.ComputerSystemDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.dao.DriveDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.COMPOSED_NODES;
import static com.intel.podm.business.entities.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetInterfaceContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asMemoryContexts;
import static com.intel.podm.business.redfish.ContextCollections.asProcessorContexts;
import static com.intel.podm.business.redfish.ContextCollections.asRemoteTargetContexts;
import static com.intel.podm.business.redfish.ContextCollections.asSimpleStorageContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.stream.Collectors.toSet;
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

    @Inject
    private DriveDao driveDao;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) {
        return new ComposedNodeCollectionDto(COMPOSED_NODES, composedNodeDao.getAllComposedNodeIds());
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
        } else {
            composedNodeDto.getProcessors().setStatus(OFFLINE_CRITICAL_STATUS);
            composedNodeDto.getMemory().setStatus(OFFLINE_CRITICAL_STATUS);
        }

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
        links.getRemoteDrives().addAll(asRemoteTargetContexts(composedNode.getRemoteTargets()));
        links.getManagedBy().addAll(asManagerContexts(single(chassisDao.getAllByChassisType(POD)).getManagers()));

        links.getLocalDrives().addAll(asDriveContexts(composedNode.getLocalDrives()));
        links.getLocalDrives().addAll(asSimpleStorageContexts(composedNode.getSimpleStorages()));
        links.getLocalDrives().addAll(asDriveContexts(composedNode.getDrives()));
    }

    private void fillNodeActions(ComposedNodeDto composedNodeDto, ComposedNode composedNode) {
        Context context = contextOf(composedNode.getId(), COMPOSED_NODE);
        ComposedNodeDto.Actions actions = composedNodeDto.getActions();
        actions.getResetAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.Reset"));
        actions.getAssembleAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.Assemble"));
        actions.getAttachEndpointAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.AttachEndpoint"));
        actions.getDetachEndpointAction().setTarget(singletonContextOf(context, "Actions/ComposedNode.DetachEndpoint"));
        actions.getDetachEndpointAction().setAllowableValues(asDriveContexts(composedNode.getDrives()).stream()
            .map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
    }

    private void fillFromComputerSystem(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        fillComposedNodeProcessorsDto(composedNodeDto, computerSystem);
        fillComposedNodeMemoryDto(composedNodeDto, computerSystem);

        fillBoot(composedNodeDto, computerSystem);

        composedNodeDto.getActions().getResetAction().setAllowableResetTypes(computerSystem.getAllowableResetTypes());
        composedNodeDto.getActions().getAttachEndpointAction().setAllowableValues(asDriveContexts(driveDao.getAchievablePcieDrives(computerSystem)).stream()
            .map(ODataIdFromContextHelper::asOdataId).collect(toSet()));

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

    private void fillComposedNodeProcessorsDto(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        ComputerSystemDto.ProcessorSummaryDto processorSummaryDto = composedNodeDto.getProcessors();
        processorSummaryDto.setCount(computerSystem.getProcessorsCount());
        processorSummaryDto.setModel(computerSystem.getProcessorsModel());
        processorSummaryDto.setStatus(computerSystem.getProcessorsStatus());
    }

    private void fillComposedNodeMemoryDto(ComposedNodeDto composedNodeDto, ComputerSystem computerSystem) {
        ComputerSystemDto.MemorySummaryDto memorySummaryDto = composedNodeDto.getMemory();
        memorySummaryDto.setTotalSystemMemoryGiB(computerSystem.getTotalSystemMemoryGiB());
        memorySummaryDto.setStatus(computerSystem.getMemoryStatus());
    }
}
