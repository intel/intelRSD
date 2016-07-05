/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto.Builder;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeMemoryDto;
import com.intel.podm.business.dto.redfish.attributes.ComposedNodeProcessorsDto;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.BootDtoHelper;
import com.intel.podm.business.services.context.Context;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.COMPOSED_NODES;
import static com.intel.podm.business.entities.redfish.components.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.business.redfish.Contexts.asLocalDriveContexts;
import static com.intel.podm.business.redfish.Contexts.asSimpleStorageContexts;
import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ComposedNodeReader {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ChassisDao chassisDao;

    @Inject
    private BootDtoHelper bootDtoHelper;

    public CollectionDto getComposedNodeCollection() {
        List<ComposedNode> composedNodes = genericDao.findAll(ComposedNode.class);
        return new ComposedNodeCollectionDto(COMPOSED_NODES, Contexts.getAsIdList(composedNodes));
    }

    public ComposedNodeDto getComposedNode(Context composedNodeContext) throws EntityNotFoundException {
        ComposedNode composedNode = getComposedNodeByContext(composedNodeContext);
        List<Manager> podManagers = single(chassisDao.getAllByChassisType(POD)).getManagers();

        Builder composedNodeBuilder = ComposedNodeDto.newBuilder()
                .id(composedNode.getId())
                .name(composedNode.getName())
                .description(composedNode.getDescription())
                .systemType(composedNode.getSystemType())
                .status(composedNode.getStatus())
                .composedNodeState(composedNode.getComposedNodeState())
                .ethernetInterfaces(Contexts.asEthernetInterfaceContexts(composedNode.getEthernetInterfaces()))
                .localDrives(asLocalDriveContexts(composedNode.getLocalDrives()))
                .simpleStorage(asSimpleStorageContexts(composedNode.getSimpleStorages()))
                .remoteDrives(Contexts.asRemoteTargetContexts(composedNode.getRemoteDrives()))
                .managedBy(Contexts.asManagerContexts(podManagers));

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
                .assetTag(computerSystem.getAssetTag())
                .manufacturer(computerSystem.getManufacturer())
                .model(computerSystem.getModel())
                .sku(computerSystem.getSku())
                .serialNumber(computerSystem.getSerialNumber())
                .partNumber(computerSystem.getPartNumber())
                .uuid(computerSystem.getUuid())
                .hostName(computerSystem.getHostName())
                .powerState(computerSystem.getPowerState())
                .biosVersion(computerSystem.getBiosVersion())
                .boot(bootDtoHelper.buildBootDto(computerSystem.getBoot()))
                .allowableResetTypes(computerSystem.getAllowableResetTypes())
                .processors(Contexts.asProcessorContexts(computerSystem.getProcessors()))
                .memoryModules(Contexts.asMemoryContexts(computerSystem.getMemoryModules()))
                .computerSystem(Contexts.toContext(computerSystem));
    }

    public ComposedNode getComposedNodeByContext(Context composedNodeContext) throws EntityNotFoundException {
        return (ComposedNode) traverser.traverse(composedNodeContext);
    }

    private ComposedNodeProcessorsDto buildComposedNodeProcessorsDto(ComputerSystem computerSystem) {
        return ComposedNodeProcessorsDto.newBuilder()
                .count(computerSystem != null ? computerSystem.getProcessorsCount() : new Integer(0))
                .model(computerSystem != null ? computerSystem.getProcessorsModel() : null)
                .status(computerSystem != null ? computerSystem.getProcessorsStatus() : OFFLINE_CRITICAL_STATUS)
                .build();
    }

    private ComposedNodeMemoryDto buildComposedNodeMemoryDto(ComputerSystem computerSystem) {
        return ComposedNodeMemoryDto.newBuilder()
                .totalSystemMemoryGiB(computerSystem != null ? computerSystem.getTotalSystemMemoryGiB() : new Integer(0))
                .status(computerSystem != null ? computerSystem.getMemoryStatus() : OFFLINE_CRITICAL_STATUS)
                .build();
    }
}
