/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.services.actions.VolumeInitializeActionInvoker;
import com.intel.podm.client.WebClientRequestException;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

import static java.lang.Boolean.TRUE;

@Dependent
public class VolumeDisassembler extends EntityDisassembler<Volume> {
    @Inject
    private VolumeInitializeActionInvoker volumeInitializeActionInvoker;

    public void setVolume(Volume volume) {
        super.entity = volume;
    }

    public void setComposedNode(ComposedNode composedNode) {
        super.node = composedNode;
    }

    @Override
    public void deallocate() {
        entity.getMetadata().setAllocated(false);
    }

    @Override
    public void unlink() {
        node.unlinkVolume(entity);
    }

    @Override
    public void removeRemoteAsset() throws WebClientRequestException {
        if (Objects.equals(entity.getEraseOnDetach(), TRUE)) {
            if (Objects.equals(entity.getInitializeActionSupported(), TRUE)) {
                try {
                    volumeInitializeActionInvoker.initialize(entity, null);
                } catch (EntityOperationException e) {
                    throw new RuntimeException(e.getMessage(), e);
                }
            }
            super.removeRemoteAsset();
        }
    }

    @Override
    public void removeEntity() {
        if (Objects.equals(entity.getEraseOnDetach(), TRUE)) {
            super.removeEntity();
        }
    }
}
