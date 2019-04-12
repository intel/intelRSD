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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class VlanTerminator {
    private final WebClientBuilder webClientBuilder;
    private final GenericDao genericDao;

    @Autowired
    public VlanTerminator(WebClientBuilder webClientBuilder, GenericDao genericDao) {
        this.webClientBuilder = webClientBuilder;
        this.genericDao = genericDao;
    }

    @Transactional(MANDATORY)
    public void deleteVlans(List<EthernetSwitchPortVlan> vlansToDelete) throws EntityOperationException {
        for (EthernetSwitchPortVlan vlan : vlansToDelete) {
            deleteVlan(vlan);
        }
    }

    @Transactional(MANDATORY)
    public void deleteVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            webClient.delete(ethernetSwitchPortVlan.getUri().toUri());
        } catch (WebClientRequestException e) {
            throw new EntityOperationException("Could not delete VLAN", e);
        }

        genericDao.remove(ethernetSwitchPortVlan);
    }
}
