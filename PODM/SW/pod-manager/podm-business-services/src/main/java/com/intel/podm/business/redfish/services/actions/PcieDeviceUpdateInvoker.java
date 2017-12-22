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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.PcieDeviceUpdateRequest;
import com.intel.podm.client.resources.redfish.PcieDeviceResource;
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
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updatePcieDevice(PcieDevice pcieDevice, PcieDeviceUpdateDefinition pcieDeviceUpdateDefinition) throws EntityOperationException {
        ExternalService service = pcieDevice.getService();
        requires(service != null, "There is no Service associated with selected PCIeDevice");

        PcieDeviceResource pcieDeviceResource = performUpdatePcieDeviceAction(pcieDeviceUpdateDefinition, service, pcieDevice);
        refreshAssetTag(pcieDeviceResource, pcieDevice);
    }

    private PcieDeviceResource performUpdatePcieDeviceAction(PcieDeviceUpdateDefinition updateDefinition, ExternalService service,
                                                             PcieDevice pcieDevice) throws EntityOperationException {
        URI chassisUri = pcieDevice.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return webClient.patchAndRetrieve(chassisUri, new PcieDeviceUpdateRequest(updateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected PCIeDevice failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), chassisUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private void refreshAssetTag(PcieDeviceResource pcieDeviceResource, PcieDevice pcieDevice) {
        pcieDevice.setAssetTag(pcieDeviceResource.getAssetTag());
    }
}
