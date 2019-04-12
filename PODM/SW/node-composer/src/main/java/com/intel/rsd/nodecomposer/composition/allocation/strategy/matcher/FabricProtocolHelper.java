/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.List;

import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class FabricProtocolHelper {
    private final VolumeHelper volumeHelper;

    @Autowired
    public FabricProtocolHelper(VolumeHelper volumeHelper) {
        this.volumeHelper = volumeHelper;
    }

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
