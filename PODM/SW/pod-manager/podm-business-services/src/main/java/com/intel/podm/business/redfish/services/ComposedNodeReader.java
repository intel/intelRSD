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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto.Builder;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeMemoryDto;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeProcessorsDto;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.DriveDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.BootDtoHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.COMPOSED_NODES;
import static com.intel.podm.business.entities.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetInterfaceContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asMemoryContexts;
import static com.intel.podm.business.redfish.ContextCollections.asProcessorContexts;
import static com.intel.podm.business.redfish.ContextCollections.asRemoteTargetContexts;
import static com.intel.podm.business.redfish.ContextCollections.asSimpleStorageContexts;
import static com.intel.podm.business.redfish.ContextCollections.getComposedNodesAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComposedNodeReader implements ReaderService<ComposedNodeDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ChassisDao chassisDao;

    @Inject
    private DriveDao driveDao;

    @Inject
    private BootDtoHelper bootDtoHelper;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) {
        List<ComposedNode> composedNodes = genericDao.findAll(ComposedNode.class);
        return new ComposedNodeCollectionDto(COMPOSED_NODES, getComposedNodesAsIdSet(composedNodes));
    }

    @Override
    public ComposedNodeDto getResource(Context composedNodeContext) throws ContextResolvingException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        Set<Manager> podManagers = single(chassisDao.getAllByChassisType(POD)).getManagers();

        Builder composedNodeBuilder = ComposedNodeDto.newBuilder()
            .id(composedNode.getId().toString())
            .name(composedNode.getName())
            .description(composedNode.getDescription())
            .status(composedNode.getStatus())
            .composedNodeState(composedNode.getComposedNodeState())
            .ethernetInterfaces(asEthernetInterfaceContexts(composedNode.getEthernetInterfaces()))
            .localDrives(asDriveContexts(composedNode.getLocalDrives()))
            .simpleStorage(asSimpleStorageContexts(composedNode.getSimpleStorages()))
            .pcieDrives(asDriveContexts(composedNode.getDrives()))
            .remoteDrives(asRemoteTargetContexts(composedNode.getRemoteTargets()))
            .managedBy(asManagerContexts(podManagers));

        return fillComputerSystemPart(composedNodeBuilder, composedNode.getComputerSystem()).build();
    }

    private Builder fillComputerSystemPart(Builder composedNodeBuilder, ComputerSystem computerSystem) {
        composedNodeBuilder
            .composedNodeProcessors(buildComposedNodeProcessorsDto(computerSystem))
            .composedNodeMemory(buildComposedNodeMemoryDto(computerSystem));

        if (computerSystem == null) {
            return composedNodeBuilder;
        }
        return composedNodeBuilder
            .uuid(computerSystem.getUuid())
            .powerState(computerSystem.getPowerState())
            .boot(bootDtoHelper.buildBootDto(computerSystem.getBoot()))
            .allowableResetTypes(computerSystem.getAllowableResetTypes())
            .processors(asProcessorContexts(computerSystem.getProcessors()))
            .memoryModules(asMemoryContexts(computerSystem.getMemoryModules()))
            .computerSystem(toContext(computerSystem))
            .availablePcieDrives(asDriveContexts(driveDao.getAchievablePcieDrives(computerSystem)));
    }

    private ComposedNodeProcessorsDto buildComposedNodeProcessorsDto(ComputerSystem computerSystem) {
        return ComposedNodeProcessorsDto.newBuilder()
            .count(computerSystem != null ? computerSystem.getProcessorsCount() : null)
            .model(computerSystem != null ? computerSystem.getProcessorModel() : null)
            .status(computerSystem != null ? computerSystem.getProcessorStatus() : OFFLINE_CRITICAL_STATUS)
            .build();
    }

    private ComposedNodeMemoryDto buildComposedNodeMemoryDto(ComputerSystem computerSystem) {
        return ComposedNodeMemoryDto.newBuilder()
            .totalSystemMemoryGiB(computerSystem != null ? computerSystem.getTotalSystemMemoryGiB() : null)
            .status(computerSystem != null ? computerSystem.getMemoryStatus() : OFFLINE_CRITICAL_STATUS)
            .build();
    }
}
