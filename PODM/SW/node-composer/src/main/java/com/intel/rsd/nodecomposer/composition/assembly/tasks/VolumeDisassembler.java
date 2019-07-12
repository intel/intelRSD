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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.VolumeInitializeActionInvoker;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static java.lang.Boolean.TRUE;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Slf4j
public class VolumeDisassembler extends EntityDisassembler<Volume> {
    private final VolumeInitializeActionInvoker volumeInitializeActionInvoker;

    @Autowired
    public VolumeDisassembler(VolumeInitializeActionInvoker volumeInitializeActionInvoker) {
        this.volumeInitializeActionInvoker = volumeInitializeActionInvoker;
    }

    public void setVolume(Volume volume) {
        super.entity = volume;
    }

    public void setComposedNode(ComposedNode composedNode) {
        super.node = composedNode;
    }

    @Override
    public void deallocate() {
        entity.setAllocated(false);
    }

    @Override
    public void unlink() {
        node.unlinkVolume(entity);
    }

    @Override
    public void decomposeRemotely() throws WebClientRequestException {
        if (!isExisting()) {
            return;
        }

        if (Objects.equals(entity.getInitializeActionSupported(), TRUE)) {
            try {
                log.debug("Initializing volume({})", entity);
                volumeInitializeActionInvoker.initialize(entity, null);
            } catch (EntityOperationException e) {
                throw new RuntimeException(e.getMessage(), e);
            }
        }
        super.decomposeRemotely();
    }
}
