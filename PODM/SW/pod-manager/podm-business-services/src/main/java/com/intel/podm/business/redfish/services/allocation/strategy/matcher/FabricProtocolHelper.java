/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.redfish.services.helpers.VolumeHelper;
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;

import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;

@Dependent
public class FabricProtocolHelper {
    @Inject
    VolumeHelper volumeHelper;

    List<Protocol> getProtocolsFromFabricEntity(Entity resourceEntity) {
        if (resourceEntity instanceof Endpoint) {
            return getProtocolsOfConnectedVolumes((Endpoint) resourceEntity);
        } else if (resourceEntity instanceof Volume) {
            return singletonList(volumeHelper.retrieveProtocolFromVolume((Volume) resourceEntity));
        } else {
            throw new IllegalStateException("Entity is neither Endpoint nor Volume.");
        }
    }

    private List<Protocol> getProtocolsOfConnectedVolumes(Endpoint endpoint) {
        return endpoint.getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Volume.class::isInstance)
            .map(Volume.class::cast)
            .map(volumeHelper::retrieveProtocolFromVolume)
            .collect(toList());
    }
}
