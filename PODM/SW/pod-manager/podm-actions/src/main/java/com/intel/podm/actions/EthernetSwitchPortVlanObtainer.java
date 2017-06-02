/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.actions;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.podm.mappers.redfish.EthernetSwitchPortVlanMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.HashSet;
import java.util.Set;

import static java.net.URI.create;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchPortVlanObtainer {
    @Inject
    private EthernetSwitchPortVlanMapper mapper;

    @Inject
    private EthernetSwitchPortVlanResourceActionsFactory actionsFactory;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Transactional(MANDATORY)
    public Set<EthernetSwitchPortVlan> discoverEthernetSwitchPortVlans(ExternalService service, Set<URI> vlanUris) throws ExternalServiceApiReaderException {
        Set<EthernetSwitchPortVlan> vlans = new HashSet<>();
        for (URI vlanUri: vlanUris) {
            vlans.add(discoverEthernetSwitchPortVlan(service, vlanUri));
        }
        return vlans;
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan discoverEthernetSwitchPortVlan(ExternalService service, URI vlanUri) throws ExternalServiceApiReaderException {
        if (service == null) {
            throw new IllegalStateException("There is no Service associated with selected switch port");
        }

        try (EthernetSwitchPortVlanResourceActions switchPortVlanActions = actionsFactory.create(service.getBaseUri())) {
            EthernetSwitchPortVlanResource psmeVlan = switchPortVlanActions.getVlan(vlanUri);
            URI relativeUri = create(vlanUri.getPath());
            EthernetSwitchPortVlan target = externalServiceDao.findOrCreateEntity(service, relativeUri, EthernetSwitchPortVlan.class);
            mapper.map(psmeVlan, target);
            return target;
        }
    }
}
