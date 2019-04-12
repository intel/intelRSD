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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.redfish.services.EndpointService;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import lombok.Setter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@SpringAware
public class TargetEndpointAssemblyTask extends NodeTask {
    private static final long serialVersionUID = 1048977484646782502L;

    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient EndpointService endpointService;
    @Autowired
    private transient VolumeHelper volumeHelper;

    @Setter
    private ODataId volumeOdataId;

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() {
        Volume volume = genericDao.find(Volume.class, volumeOdataId);
        Fabric fabric = volumeHelper.retrieveFabricFromVolume(volume);
        endpointService.createTargetEndpoint(volumeOdataId, getComposedNode(), fabric);
    }

    private ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, composedNodeODataId);
    }
}
