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
import com.intel.podm.business.dto.redfish.RemoteTargetDto;
import com.intel.podm.business.dto.redfish.RemoteTargetIscsiAddressDto;
import com.intel.podm.business.dto.redfish.attributes.ChapDto;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.embeddables.Chap;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.REMOTE_TARGETS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class RemoteTargetServiceImpl implements ReaderService<RemoteTargetDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context serviceContext) throws ContextResolvingException {
        StorageService storageService = (StorageService) traverser.traverse(serviceContext);
        return new CollectionDto(REMOTE_TARGETS, getAsIdSet(storageService.getRemoteTargets()));
    }

    @Override
    public RemoteTargetDto getResource(Context remoteTargetContext) throws ContextResolvingException {
        RemoteTarget remoteTarget = (RemoteTarget) traverser.traverse(remoteTargetContext);
        LogicalDrive logicalDrive = remoteTarget.getLogicalDrives().stream().findAny().orElse(null);

        return RemoteTargetDto.newBuilder()
            .id(remoteTargetContext.getId().toString())
            .name(remoteTarget.getName())
            .description(remoteTarget.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(remoteTarget.getService(), remoteTarget.getUnknownOems()))
            .status(remoteTarget.getStatus())
            .type(remoteTarget.getType())
            .iscsiAddresses(buildRemoteTargetIscsiAddresses(remoteTarget.getRemoteTargetIscsiAddresses(), toContext(logicalDrive)))
            .iscsiInitiatorIqn(remoteTarget.getIscsiInitiatorIqn())
            .build();
    }

    private List<RemoteTargetIscsiAddressDto> buildRemoteTargetIscsiAddresses(Collection<RemoteTargetIscsiAddress> remoteTargetIscsiAddresses,
                                                                              Context logicalDriveContext) {
        return remoteTargetIscsiAddresses
            .stream()
            .map(remoteTargetIscsiAddress -> toRemoteTargetIscsiAddressDto(remoteTargetIscsiAddress, logicalDriveContext))
            .collect(toList());
    }

    private RemoteTargetIscsiAddressDto toRemoteTargetIscsiAddressDto(RemoteTargetIscsiAddress remoteTargetIscsiAddress, Context logicalDriveContext) {
        return RemoteTargetIscsiAddressDto.newBuilder()
            .targetIqn(remoteTargetIscsiAddress.getTargetIqn())
            .targetLun(buildTargetLunDtos(remoteTargetIscsiAddress.getTargetLuns(), logicalDriveContext))
            .targetPortalIp(remoteTargetIscsiAddress.getTargetPortalIp())
            .targetPortalPort(remoteTargetIscsiAddress.getTargetPortalPort())
            .chap(toChapDto(remoteTargetIscsiAddress.getChap()))
            .build();
    }

    private List<RemoteTargetIscsiAddressDto.TargetLunDto> buildTargetLunDtos(List<Integer> targetLuns, Context logicalDriveContext) {
        return targetLuns
            .stream()
            .map(targetLun -> new RemoteTargetIscsiAddressDto.TargetLunDto(targetLun, logicalDriveContext))
            .collect(toList());
    }

    public ChapDto toChapDto(Chap chap) {
        if (chap == null) {
            return null;
        }

        return ChapDto
            .newBuilder()
            .type(chap.getType())
            .username(chap.getUsername())
            .mutualUsername(chap.getMutualUsername())
            .build();
    }
}
