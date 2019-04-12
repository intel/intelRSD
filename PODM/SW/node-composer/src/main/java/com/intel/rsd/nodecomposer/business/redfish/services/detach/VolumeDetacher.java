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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.AttachableType.Types.VOLUME;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component(VOLUME)
@Scope(SCOPE_SINGLETON)
public class VolumeDetacher extends Detacher<Volume> {
    private final VolumeHelper volumeHelper;

    @Autowired
    public VolumeDetacher(VolumeHelper volumeHelper) {
        this.volumeHelper = volumeHelper;
    }

    @Override
    protected Fabric retrieveFabricFromAsset(Volume volume) {
        return volumeHelper.retrieveFabricFromVolume(volume);
    }

    @Override
    protected void validate(ComposedNode composedNode, Volume volume) throws RequestValidationException {
        if (!composedNode.getVolumes().contains(volume)) {
            throw new RequestValidationException(createWithViolations("Provided volume is not attached to this Node."));
        }

        if (ISCSI.equals(volumeHelper.retrieveProtocolFromVolume(volume))) {
            throw new RequestValidationException(createWithViolations("Detach volume with iSCSI protocol is not allowed."));
        }
    }

    @Override
    protected void performResourceSpecificActions(ComposedNode composedNode, Volume volume) {
    }
}
