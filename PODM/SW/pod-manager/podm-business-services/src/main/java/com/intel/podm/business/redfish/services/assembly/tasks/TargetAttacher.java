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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.dao.RemoteTargetDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.redfish.services.assembly.AssemblyException;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class TargetAttacher {
    @Inject
    private GenericDao genericDao;

    @Inject
    private RemoteTargetDao remoteTargetDao;

    @Transactional(REQUIRES_NEW)
    public boolean attachIfCompleted(Id nodeId, UUID rssServiceUuid, URI newRemoteTargetUri) throws AssemblyException {
        List<RemoteTarget> remoteTargets = remoteTargetDao.getAllBySourceUri(newRemoteTargetUri).stream()
                .filter(remoteTarget -> Objects.equals(rssServiceUuid, remoteTarget.getService().getUuid()))
                .collect(toList());

        if (remoteTargets.isEmpty()) {
            return false;
        }

        if (remoteTargets.size() != 1) {
            throw new AssemblyException(
                    format("More than one remote target with URI '%s' for service '%s' is not allowed", newRemoteTargetUri, rssServiceUuid));
        }

        RemoteTarget remoteTarget = single(remoteTargets);
        remoteTarget.getMetadata().setAllocated(true);
        ComposedNode node = genericDao.find(ComposedNode.class, nodeId);
        node.addRemoteTarget(remoteTarget);
        node.setAssociatedStorageServiceUuid(remoteTarget.getService().getUuid());
        node.setAssociatedRemoteTargetIqn(remoteTarget.getTargetIqn());
        return true;
    }
}
