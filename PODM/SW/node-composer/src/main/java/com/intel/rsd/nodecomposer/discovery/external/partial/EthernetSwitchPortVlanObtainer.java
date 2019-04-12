/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.partial;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.rsd.nodecomposer.mappers.redfish.EthernetSwitchPortVlanMapper;
import com.intel.rsd.nodecomposer.persistence.dao.DiscoverableEntityDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.HashSet;
import java.util.Set;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EthernetSwitchPortVlanObtainer {
    private final EthernetSwitchPortVlanMapper mapper;
    private final WebClientBuilder webClientBuilder;
    private final DiscoverableEntityDao discoverableEntityDao;

    @Autowired
    public EthernetSwitchPortVlanObtainer(EthernetSwitchPortVlanMapper mapper, WebClientBuilder webClientBuilder, DiscoverableEntityDao discoverableEntityDao) {
        this.mapper = mapper;
        this.webClientBuilder = webClientBuilder;
        this.discoverableEntityDao = discoverableEntityDao;
    }

    @Transactional(MANDATORY)
    public Set<EthernetSwitchPortVlan> discoverEthernetSwitchPortVlans(Set<URI> vlanUris) throws WebClientRequestException {
        Set<EthernetSwitchPortVlan> vlans = new HashSet<>();
        for (URI vlanUri : vlanUris) {
            vlans.add(discoverEthernetSwitchPortVlan(vlanUri));
        }
        return vlans;
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan discoverEthernetSwitchPortVlan(URI vlanUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            EthernetSwitchPortVlanResource psmeVlan = (EthernetSwitchPortVlanResource) webClient.get(vlanUri);
            URI relativeUri = URI.create(vlanUri.getPath());
            EthernetSwitchPortVlan target = (EthernetSwitchPortVlan) discoverableEntityDao.findOrCreateEntity(relativeUri, EthernetSwitchPortVlan.class);
            mapper.map(psmeVlan, target);
            return target;
        }
    }
}
