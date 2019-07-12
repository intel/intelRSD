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

package com.intel.rsd.nodecomposer.discovery.external.events;

import com.intel.rsd.nodecomposer.Dirtying;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.VolumeResource;
import com.intel.rsd.nodecomposer.mappers.redfish.VolumeMapper;
import com.intel.rsd.nodecomposer.persistence.dao.DiscoverableEntityDao;
import com.intel.rsd.nodecomposer.persistence.redfish.StorageService;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.context.ApplicationListener;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromString;

@Component
@Slf4j
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class VolumeDiscoverer implements ApplicationListener<VolumeCreatedEvent> {
    private final WebClientBuilder webClientBuilder;
    private final DiscoverableEntityDao discoverableEntityDao;
    private final VolumeMapper volumeMapper;

    public VolumeDiscoverer(WebClientBuilder webClientBuilder, DiscoverableEntityDao discoverableEntityDao, VolumeMapper volumeMapper) {
        this.webClientBuilder = webClientBuilder;
        this.discoverableEntityDao = discoverableEntityDao;
        this.volumeMapper = volumeMapper;
    }

    @Override
    @Dirtying
    @Transactional(Transactional.TxType.REQUIRES_NEW)
    public void onApplicationEvent(VolumeCreatedEvent event) {
        try (val webClient = webClientBuilder.createResourceManagerInstance().build()) {
            val volumeResource = (VolumeResource) webClient.get(event.getUri());
            String volumeUri = volumeResource.getOdataId();

            URI storageServiceUri = getStorageServiceUri(volumeUri);
            StorageService storageService = (StorageService) discoverableEntityDao.findEntityOrNull(storageServiceUri);
            if (storageService == null) {
                log.warn("Storage service ({}) was not available during volume ({}) discovery.", storageServiceUri, volumeUri);
                return;
            }
            Volume volume = discoverableEntityDao.createEntity(oDataIdFromString(volumeUri), Volume.class);
            volumeMapper.map(volumeResource, volume);
            volume.setStorageService(storageService);
        } catch (WebClientRequestException e) {
            log.error("Could not discover volume: {}", event, e);
        }
    }

    private URI getStorageServiceUri(String volumeUri) {
        String volumesUri = volumeUri.substring(0, volumeUri.lastIndexOf("/"));
        return URI.create(volumesUri.substring(0, volumesUri.lastIndexOf("/")));
    }
}
