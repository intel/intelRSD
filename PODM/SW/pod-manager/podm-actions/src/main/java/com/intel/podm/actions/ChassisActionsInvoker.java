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

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.ChassisResourceActions;
import com.intel.podm.client.api.actions.ChassisResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.BaseChassisUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ChassisActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private ChassisResourceActionsFactory chassisResourceActionsFactory;

    @Transactional(MANDATORY)
    public void updateChassis(Chassis chassis, BaseChassisUpdateDefinition chassisUpdateDefinition) throws ActionException {
        URI serviceBaseUri = getServiceBaseUri(chassis);
        URI chassisUri = chassis.getSourceUri();

        try (ChassisResourceActions actions = chassisResourceActionsFactory.create(serviceBaseUri)) {
            actions.update(chassisUri, chassisUpdateDefinition);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update on selected Chassis failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, chassisUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Update action was successful, but refreshing selected Chassis failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, chassisUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

    @Transactional(MANDATORY)
    public boolean chassisHasLocationId(Chassis chassis) throws ActionException {
        URI serviceBaseUri = getServiceBaseUri(chassis);
        URI chassisUri = chassis.getSourceUri();

        try (ChassisResourceActions actions = chassisResourceActionsFactory.create(serviceBaseUri)) {
            ChassisResource chassisResource = actions.get(chassisUri);

            if (chassisResource.getRackChassisAttributes() != null) {
                return chassisResource.getRackChassisAttributes().getLocation().getId().isAssigned();
            } else {
                return chassisResource.getLocationId().isAssigned();
            }
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Retrieving of selected Chassis failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, chassisUri);
            throw new ActionException(errorMessage, e.getErrorResponse());
        }
    }

    private URI getServiceBaseUri(Chassis chassis) {
        ExternalService service = chassis.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected Chassis");

        return service.getBaseUri();
    }
}
