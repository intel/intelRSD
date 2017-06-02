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

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Resettable;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.actions.ResetAction;
import com.intel.podm.client.api.actions.ResetActionFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ResetType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.types.actions.ResetType.FORCE_OFF;
import static com.intel.podm.common.types.actions.ResetType.GRACEFUL_SHUTDOWN;
import static com.intel.podm.common.types.actions.ResetType.ON;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

/**
 * Delegates reset action performed on Computer system.
 */
@Dependent
public class ResetActionInvoker {

    private static final String RESET_URL = "/Actions/%s.Reset";

    @Inject
    private ResetActionFactory resetActionFactory;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void reset(Resettable resettableEntity, ResetType resetType) throws ActionException {
        ExternalService service = resettableEntity.getService();
        requires(service != null, "There is no Service associated with selected Switch");

        try (ResetAction actions = createResourceActionActions(service)) {
            URI resourceResetUri = getResettableEntityUriForResetAction(resettableEntity);
            actions.reset(resourceResetUri, resetType);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = format("Resource %s reset action (%s) failed", resettableEntity.getId(), resetType);
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), resettableEntity.getSourceUri());
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

    @Transactional(MANDATORY)
    public void powerOn(Resettable resettableEntity) throws ActionException {
        reset(resettableEntity, ON);
    }

    @Transactional(MANDATORY)
    public void shutdownGracefully(Resettable resettableEntity) throws ActionException {
        reset(resettableEntity, GRACEFUL_SHUTDOWN);
    }

    @Transactional(MANDATORY)
    public void powerOff(Resettable resettableEntity) throws ActionException {
        reset(resettableEntity, FORCE_OFF);
    }

    private ResetAction createResourceActionActions(ExternalService service) throws ActionException {
        return resetActionFactory.create(service.getBaseUri());
    }

    private URI getResettableEntityUriForResetAction(Resettable resettableEntity) {
        return URI.create(resettableEntity.getSourceUri() + String.format(RESET_URL, resettableEntity.getResetName()));
    }
}
