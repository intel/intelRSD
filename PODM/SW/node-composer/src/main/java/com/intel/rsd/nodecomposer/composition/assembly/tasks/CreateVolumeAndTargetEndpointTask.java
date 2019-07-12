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
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.EndpointService;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.PostConstruct;
import java.io.Serializable;
import java.util.function.Consumer;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;
import static org.springframework.transaction.annotation.Propagation.NEVER;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class CreateVolumeAndTargetEndpointTask extends NodeTask implements Serializable {

    private static final long serialVersionUID = 1325767748473248086L;

    @Setter
    private RemoteDriveAllocationContextDescriptor resourceDescriptor;

    @Autowired
    private transient GenericDao genericDao;

    @Autowired
    private transient VolumeService volumeService;

    @Autowired
    private transient EndpointService endpointService;

    private transient Consumer<AttachableAsset> assetToNodeAttacher;

    @PostConstruct
    public void init() {
        this.assetToNodeAttacher = attachableAsset -> genericDao.find(ComposedNode.class, composedNodeODataId).attachAsset(attachableAsset);
    }

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(propagation = NEVER)
    public void run() {
        val storageServiceODataId = resourceDescriptor.getStorageServiceODataId();
        val fabric = findFabric(storageServiceODataId);

        try {
            val createdVolumeOdataId = volumeService.createVolumeAndWaitUntilEnabled(resourceDescriptor);
            volumeService.discoverCreatedVolume(storageServiceODataId, createdVolumeOdataId, assetToNodeAttacher);

            val createdEndpointOdataId = endpointService.createTargetEndpoint(fabric.getUri(), fabric.getFabricType(), createdVolumeOdataId);
            endpointService.discoverEndpoint(fabric.getUri(), createdEndpointOdataId, assetToNodeAttacher);
        } catch (BusinessApiException e) {
            throw new RuntimeException(e);
        }
    }

    private Fabric findFabric(ODataId storageServiceODataId) {
        val storageService = genericDao.find(StorageService.class, storageServiceODataId);
        return storageService.getFabric();
    }
}
