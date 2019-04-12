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

package com.intel.rsd.nodecomposer.discovery.handlers;

import com.intel.rsd.nodecomposer.discovery.external.finalizers.ComposedNodeDisableService;
import com.intel.rsd.nodecomposer.persistence.handlers.OnDeleteVolumeHandler;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class OnDeleteVolumeHandlerImpl implements OnDeleteVolumeHandler {
    private final ComposedNodeDisableService composedNodeDisableService;

    @Autowired
    public OnDeleteVolumeHandlerImpl(ComposedNodeDisableService composedNodeDisableService) {
        this.composedNodeDisableService = composedNodeDisableService;
    }

    @Override
    @Transactional(MANDATORY)
    public void preRemove(Volume volume) {
        ComposedNode composedNode = volume.getComposedNode();
        if (composedNode != null) {
            composedNodeDisableService.disableComposedNode(composedNode);
        }
    }
}
