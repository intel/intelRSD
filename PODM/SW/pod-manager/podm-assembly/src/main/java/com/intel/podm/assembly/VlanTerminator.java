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

package com.intel.podm.assembly;

import com.intel.podm.actions.ActionException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortVlanResourceActionsFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class VlanTerminator {
    @Inject
    private EthernetSwitchPortVlanResourceActionsFactory actionsFactory;

    @Inject
    private GenericDao genericDao;

    @Transactional(MANDATORY)
    public void deleteVlans(List<EthernetSwitchPortVlan> vlansToDelete) throws ActionException {
        for (EthernetSwitchPortVlan vlan : vlansToDelete) {
            deleteVlan(vlan);
        }
    }

    @Transactional(MANDATORY)
    public void deleteVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) throws ActionException {
        ExternalService service = ethernetSwitchPortVlan.getService();
        if (service == null) {
            throw new IllegalStateException("There is no Service associated with selected ethernet switch port vlan");
        }


        try (EthernetSwitchPortVlanResourceActions switchPortVlanActions = actionsFactory.create(service.getBaseUri())) {
            switchPortVlanActions.deleteVlan(ethernetSwitchPortVlan.getSourceUri());
        } catch (ExternalServiceApiActionException e) {
            throw new ActionException("Could not delete vlan", e.getErrorResponse(), e);
        }

        genericDao.remove(ethernetSwitchPortVlan);
    }
}
