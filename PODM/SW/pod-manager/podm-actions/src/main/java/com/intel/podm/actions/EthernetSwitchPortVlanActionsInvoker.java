/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.client.api.RedfishApiException;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActionsFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.VlanCreationRequest;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchPortVlanActionsInvoker {
    @Inject
    private EthernetSwitchPortVlanResourceActionsFactory actionsFactory;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private Logger logger;

    @Inject
    private EthernetSwitchPortVlanObtainer vlanObtainer;

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan create(EthernetSwitchPort port, VlanCreationRequest request) throws ActionException {
        ExternalService service = port.getService();
        if (service == null) {
            throw new IllegalStateException("There is no Service associated with selected switch port");
        }

        URI switchPortUri = port.getSourceUri();

        try (EthernetSwitchPortVlanResourceActions switchPortVlanActions = actionsFactory.create(service.getBaseUri())) {
            URI vlanUri = switchPortVlanActions.createVlan(switchPortUri, request.getId(), request.isTagged(), request.isEnabled());
            return vlanObtainer.discoverEthernetSwitchPortVlan(service, vlanUri);
        } catch (RedfishApiException e) {
            String errorMessage = "Vlan creation failed on selected switch port";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchPortUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }
}
