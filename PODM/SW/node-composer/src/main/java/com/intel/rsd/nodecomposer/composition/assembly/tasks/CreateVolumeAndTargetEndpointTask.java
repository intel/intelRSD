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
import com.intel.rsd.nodecomposer.business.redfish.services.StateService;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.rsd.nodecomposer.discovery.external.partial.StorageServiceVolumeObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;
import java.net.URI;

import static javax.transaction.Transactional.TxType.NEVER;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class CreateVolumeAndTargetEndpointTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 1325767748473248086L;

    @Setter
    private RemoteDriveAllocationContextDescriptor resourceDescriptor;
    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient StorageServiceVolumeObtainer volumeObtainer;
    @Autowired
    private transient EndpointService endpointService;
    @Autowired
    private transient VolumeHelper volumeHelper;
    @Autowired
    private transient StateService stateService;
    @Autowired
    private CreateVolumeTask createVolumeTask;
    @Autowired
    private transient RequiresNewTransactionWrapper requiresNewTransactionWrapper;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(NEVER)
    public void run() throws RuntimeException {
        ODataId storageServiceODataId = resourceDescriptor.getStorageServiceODataId();
        URI newVolumeUri = createVolumeTask.createVolumeAndReturnItsUri(resourceDescriptor);
        stateService.waitForEnabledStateOnVolume(newVolumeUri);

        requiresNewTransactionWrapper.run(() -> {
            Volume volume = discoverVolumeOnStorage(storageServiceODataId, newVolumeUri);
            ComposedNode composedNode = genericDao.find(ComposedNode.class, composedNodeODataId);
            addToNode(composedNode, volume);
            Fabric fabric = volumeHelper.retrieveFabricFromVolume(volume);
            endpointService.createTargetEndpoint(volume.getUri(), composedNode, fabric);
            return null;
        });

    }

    private Volume discoverVolumeOnStorage(ODataId storageServiceODataId, URI newVolumeUri) {
        try {
            StorageService storageService = getStorageServiceFromODataId(storageServiceODataId);
            URI relativeVolumeUri = URI.create(newVolumeUri.getPath());
            return volumeObtainer.discoverStorageServiceVolume(storageService, relativeVolumeUri);
        } catch (WebClientRequestException e) {
            log.error("Volume obtain failed for Node: {}, details: {}", composedNodeODataId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void addToNode(ComposedNode composedNode, Volume volume) {
        composedNode.attachAsset(volume);
    }

    private StorageService getStorageServiceFromODataId(ODataId storageServiceODataId) {
        return genericDao.find(StorageService.class, storageServiceODataId);
    }
}
