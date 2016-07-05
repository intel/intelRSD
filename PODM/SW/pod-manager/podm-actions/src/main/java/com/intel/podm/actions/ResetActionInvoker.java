/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.actions.ComputerSystemResourceActions;
import com.intel.podm.client.api.actions.ComputerSystemResourceActionsFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ResetType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.google.common.base.Preconditions.checkState;
import static com.intel.podm.common.types.actions.ResetType.FORCE_OFF;
import static com.intel.podm.common.types.actions.ResetType.GRACEFUL_SHUTDOWN;
import static com.intel.podm.common.types.actions.ResetType.ON;
import static java.lang.String.format;

/**
 * Delegates reset action performed on Computer system.
 */
@Dependent
public class ResetActionInvoker {
    @Inject
    private ComputerSystemResourceActionsFactory actionsFactory;

    @Inject
    private Logger logger;

    public void powerOn(ComputerSystem computerSystem) throws ActionException {
        reset(computerSystem, ON);
    }

    public void reset(ComputerSystem computerSystem, ResetType resetType) throws ActionException {
        ExternalService service = computerSystem.getService();
        checkState(service != null, "There is no Service associated with selected ComputerSystem");
        try {
            ComputerSystemResourceActions actions = createPsmeComputerSystemActions(computerSystem);
            actions.reset(computerSystem.getSourceUri(), resetType);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = format("ComputerSystem %s reset action (%s) failed", computerSystem.getId(), resetType);
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), computerSystem.getSourceUri());
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

    private ComputerSystemResourceActions createPsmeComputerSystemActions(ComputerSystem computerSystem) {
        ExternalService service = computerSystem.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected ComputerSystem");
        }
        return actionsFactory.create(service.getBaseUri());
    }

    public void shutdownGracefully(ComputerSystem computerSystem) throws ActionException {
        reset(computerSystem, GRACEFUL_SHUTDOWN);
    }

    public void powerOff(ComputerSystem computerSystem) throws ActionException {
        reset(computerSystem, FORCE_OFF);
    }
}
