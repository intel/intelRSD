/*
 * Copyright (c) 2015-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EthernetSwitchPortResource;
import com.intel.rsd.nodecomposer.mappers.redfish.EthernetSwitchPortMapper;
import com.intel.rsd.nodecomposer.persistence.dao.DiscoverableEntityDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier.getUrisFromResources;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortObtainer {
    private final WebClientBuilder webClientBuilder;
    private final EthernetSwitchPortMapper switchPortMapper;
    private final DiscoverableEntityDao discoverableEntityDao;
    private final EthernetSwitchPortVlanObtainer vlanObtainer;

    @Autowired
    public EthernetSwitchPortObtainer(WebClientBuilder webClientBuilder, EthernetSwitchPortMapper switchPortMapper, DiscoverableEntityDao discoverableEntityDao,
                                      EthernetSwitchPortVlanObtainer vlanObtainer) {
        this.webClientBuilder = webClientBuilder;
        this.switchPortMapper = switchPortMapper;
        this.discoverableEntityDao = discoverableEntityDao;
        this.vlanObtainer = vlanObtainer;
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPort discoverPort(URI portUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            EthernetSwitchPortResource switchPortResource = (EthernetSwitchPortResource) webClient.get(portUri);
            EthernetSwitchPort targetSwitchPort = readEthernetSwitchPortResource(portUri, switchPortResource);

            Set<EthernetSwitchPortVlan> vlans = vlanObtainer.discoverEthernetSwitchPortVlans(getUrisFromResources(switchPortResource.getVlans()));
            vlans.forEach(targetSwitchPort::addEthernetSwitchPortVlan);

            setPrimaryVlanInSwitchPort(switchPortResource, targetSwitchPort, vlans);

            return targetSwitchPort;
        }
    }

    private EthernetSwitchPort readEthernetSwitchPortResource(URI switchPortUri,
                                                              EthernetSwitchPortResource switchPortResource) {
        URI sourceSwitchPortUri = URI.create(switchPortUri.getPath());
        EthernetSwitchPort targetSwitchPort = (EthernetSwitchPort) discoverableEntityDao.findOrCreateEntity(sourceSwitchPortUri, EthernetSwitchPort.class);
        switchPortMapper.map(switchPortResource, targetSwitchPort);
        return targetSwitchPort;
    }

    private void setPrimaryVlanInSwitchPort(EthernetSwitchPortResource switchPortResource, EthernetSwitchPort targetPort, Set<EthernetSwitchPortVlan> vlans)
        throws WebClientRequestException {

        EthernetSwitchPortVlan oldPvid = targetPort.getPrimaryVlan();
        ResourceSupplier potentiallyNewPvid = switchPortResource.getPrimaryVlan();
        if (potentiallyNewPvid == null) {
            targetPort.setPrimaryVlan(null);
        } else {
            EthernetSwitchPortVlan newPrimaryVlan = vlans.stream()
                .filter(vlan -> Objects.equals(vlan.getUri().toUri(), potentiallyNewPvid.getUri()))
                .collect(toSingle());

            if (!Objects.equals(oldPvid, newPrimaryVlan)) {
                targetPort.setPrimaryVlan(newPrimaryVlan);
            }
        }
    }
}
