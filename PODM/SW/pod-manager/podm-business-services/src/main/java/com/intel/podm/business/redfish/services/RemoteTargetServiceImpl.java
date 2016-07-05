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
import com.intel.podm.business.dto.redfish.RemoteTargetDto;
import com.intel.podm.business.dto.redfish.RemoteTargetIscsiAddressDto;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.properties.RemoteTargetIscsiAddress;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.RemoteTargetService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.REMOTE_TARGETS;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class RemoteTargetServiceImpl implements RemoteTargetService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getRemoteTargetsCollection(Context serviceContext) throws EntityNotFoundException {
        StorageService storageService = (StorageService) traverser.traverse(serviceContext);
        return new CollectionDto(REMOTE_TARGETS, Contexts.getAsIdList(storageService.getRemoteTargets()));
    }

    @Override
    public RemoteTargetDto getRemoteTarget(Context remoteTargetContext) throws EntityNotFoundException {
        RemoteTarget remoteTarget = (RemoteTarget) traverser.traverse(remoteTargetContext);
        LogicalDrive logicalDrive = remoteTarget.getLogicalDrives().stream().findAny().orElse(null);

        return RemoteTargetDto.newBuilder()
                .id(remoteTargetContext.getId())
                .name(remoteTarget.getName())
                .description(remoteTarget.getDescription())
                .status(remoteTarget.getStatus())
                .type(remoteTarget.getType())
                .iscsiAddresses(buildRemoteTargetIscsiAddresses(remoteTarget.getRemoteTargetIscsiAddresses(), Contexts.toContext(logicalDrive)))
                .iscsiInitiatorIqn(remoteTarget.getIscsiInitiatorIqn())
                .build();
    }

    private List<RemoteTargetIscsiAddressDto> buildRemoteTargetIscsiAddresses(List<RemoteTargetIscsiAddress> remoteTargetIscsiAddresses,
                                                                              Context logicalDriveContext) {
        return remoteTargetIscsiAddresses
                .stream()
                .map(remoteTargetIscsiAddress -> toRemoteTargetIscsiAddressDto(remoteTargetIscsiAddress, logicalDriveContext))
                .collect(toList());
    }

    private RemoteTargetIscsiAddressDto toRemoteTargetIscsiAddressDto(RemoteTargetIscsiAddress remoteTargetIscsiAddress, Context logicalDriveContext) {
        return RemoteTargetIscsiAddressDto.newBuilder()
                .targetIqn(remoteTargetIscsiAddress.getTargetIqn())
                .targetLun(buildTargetLunDtos(remoteTargetIscsiAddress.getTargetLun(), logicalDriveContext))
                .targetPortalIp(remoteTargetIscsiAddress.getTargetPortalIp())
                .targetPortalPort(remoteTargetIscsiAddress.getTargetPortalPort())
                .build();
    }

    private List<RemoteTargetIscsiAddressDto.TargetLunDto> buildTargetLunDtos(List<Integer> targetLuns, Context logicalDriveContext) {
        return targetLuns
                .stream()
                .map(targetLun -> new RemoteTargetIscsiAddressDto.TargetLunDto(targetLun, logicalDriveContext))
                .collect(toList());
    }


}
