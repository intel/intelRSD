/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachActionValidator;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.TargetEndpointFinder;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.redfish.services.attach.Attacher.ATTACHER_BEAN_PREFIX;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.VOLUME;

@Component(ATTACHER_BEAN_PREFIX + VOLUME)
public class VolumeAttacher extends Attacher<Volume> {

    private final VolumeHelper volumeHelper;

    @Autowired
    public VolumeAttacher(AttachActionValidator<Volume> attachActionValidator, TargetEndpointFinder targetEndpointFinder,
                          RemoteAttacher remoteAttacher, LocalAttacher localAttacher,
                          VolumeHelper volumeHelper) {

        super(attachActionValidator, targetEndpointFinder, remoteAttacher, localAttacher);
        this.volumeHelper = volumeHelper;
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Volume volume) {
        return volumeHelper.retrieveFabricFromVolume(volume);
    }

    @Override
    protected Protocol retrieveAssetProtocol(Volume volume) {
        return volumeHelper.retrieveProtocolFromVolume(volume);
    }

    @Override
    public Violations getAssetSpecificViolations(ComposedNode composedNode, Volume volume) {
        Violations violations = new Violations();

        if (!volume.getEndpoints().isEmpty() && volume.getEndpoints().stream().noneMatch(Endpoint::isAttachable)) {
            violations.addViolation("All endpoints attached to selected volume are in use.");
        }

        return violations;
    }

}
