/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.EthernetSwitchPortVlanMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.net.URI;

import static java.net.URI.create;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
public class EthernetSwitchPortVlanObtainer {
    @Inject
    private EthernetSwitchPortVlanMapper mapper;

    @Inject
    private EthernetSwitchPortResourceActionsFactory resourceActionsFactory;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Transactional(REQUIRED)
    public EthernetSwitchPortVlan discoverNewEthernetSwitchPortVlan(EthernetSwitchPort port, URI vlanUri) throws ExternalServiceApiReaderException {
        return discoverVlan(port, vlanUri);
    }

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public EthernetSwitchPortVlan discoverEthernetSwitchPortVlan(Id portId, URI vlanUri) throws ExternalServiceApiReaderException {
        EthernetSwitchPort port = ethernetSwitchPortDao.getOrThrow(portId);
        return discoverVlan(port, vlanUri);
    }

    private EthernetSwitchPortVlan discoverVlan(EthernetSwitchPort port, URI vlanUri) throws ExternalServiceApiReaderException {
        ExternalService service = port.getService();
        if (service == null) {
            throw new IllegalStateException("There is no Service associated with selected switch port");
        }

        EthernetSwitchPortResourceActions switchPortActions = resourceActionsFactory.create(service.getBaseUri());
        EthernetSwitchPortVlanResource psmeVlan = switchPortActions.getVlan(vlanUri);
        URI relativeUri = create(vlanUri.getPath());
        EthernetSwitchPortVlan target = service.findOrCreate(relativeUri, EthernetSwitchPortVlan.class);
        mapper.map(psmeVlan, target);
        port.addVlan(target);
        return target;
    }
}
