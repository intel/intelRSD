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

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.PcieDeviceResourceActions;
import com.intel.podm.client.api.actions.PcieDeviceResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.PcieDeviceResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.PcieDeviceUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class PcieDeviceUpdateInvoker {
    @Inject
    private PcieDeviceResourceActionsFactory pcieDeviceResourceActionsFactory;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updatePcieDevice(PcieDevice pcieDevice, PcieDeviceUpdateDefinition pcieDeviceUpdateDefinition) throws ActionException {
        ExternalService service = pcieDevice.getService();
        requires(service != null, "There is no Service associated with selected PCIeDevice");

        URI baseUri = service.getBaseUri();
        URI chassisUri = pcieDevice.getSourceUri();

        try (PcieDeviceResourceActions actions = pcieDeviceResourceActionsFactory.create(baseUri)) {
            PcieDeviceResource pcieDeviceResource = actions.update(chassisUri, pcieDeviceUpdateDefinition);
            pcieDevice.setAssetTag(pcieDeviceResource.getAssetTag());
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update on selected PCIeDevice failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), chassisUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Update action was successful, but refreshing selected PCIeDevice failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), chassisUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }
}
