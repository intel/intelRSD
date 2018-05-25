/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.ChassisUpdateRequest;
import com.intel.podm.client.actions.LegacyRackChassisUpdateRequest;
import com.intel.podm.client.resources.redfish.ChassisResource;
import com.intel.podm.client.resources.redfish.LocationObject;
import com.intel.podm.client.resources.redfish.RackScaleRackChassisOem;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.BaseChassisUpdateDefinition;
import com.intel.podm.common.types.actions.ChassisUpdateDefinition;
import com.intel.podm.common.types.actions.LegacyRackChassisUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ChassisActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Transactional(MANDATORY)
    public void updateChassis(ExternalLink externalLink, BaseChassisUpdateDefinition chassisUpdateDefinition) throws EntityOperationException {
        URI serviceBaseUri = externalLink.getExternalService().getBaseUri();
        URI chassisUri = externalLink.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            Object chassisUpdateRequest;
            if (chassisUpdateDefinition instanceof LegacyRackChassisUpdateDefinition) {
                chassisUpdateRequest = new LegacyRackChassisUpdateRequest((LegacyRackChassisUpdateDefinition) chassisUpdateDefinition);
            } else {
                chassisUpdateRequest = new ChassisUpdateRequest((ChassisUpdateDefinition) chassisUpdateDefinition);
            }

            webClient.patchAndRetrieve(chassisUri, chassisUpdateRequest);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Chassis failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, chassisUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public boolean chassisHasLocationId(ExternalLink externalLink) throws EntityOperationException {
        URI serviceBaseUri = externalLink.getExternalService().getBaseUri();
        URI chassisUri = externalLink.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            ChassisResource chassisResource = (ChassisResource) webClient.get(chassisUri);
            RackScaleRackChassisOem rackChassisAttributes = chassisResource.getRackChassisAttributes();
            if (rackChassisAttributes != null) {
                LocationObject locationObject = rackChassisAttributes.getLocation();
                return locationObject != null && locationObject.getId().isAssigned();
            } else {
                return chassisResource.getLocationId().isAssigned();
            }
        } catch (WebClientRequestException e) {
            String errorMessage = "Retrieving of selected Chassis failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, chassisUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
