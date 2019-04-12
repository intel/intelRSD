/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.ResetRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ResetType;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.nodecomposer.types.actions.ResetType.FORCE_OFF;
import static com.intel.rsd.nodecomposer.types.actions.ResetType.GRACEFUL_SHUTDOWN;
import static com.intel.rsd.nodecomposer.types.actions.ResetType.ON;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

/**
 * Delegates reset action performed on Computer system.
 */
@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ComputerSystemResetActionInvoker {
    private static final String RESET_URL = "/Actions/%s.Reset";

    private final WebClientBuilder webClientBuilder;

    @Autowired
    public ComputerSystemResetActionInvoker(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    @Transactional(MANDATORY)
    public void reset(ComputerSystem computerSystem, ResetType resetType) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            URI resourceResetUri = getResettableEntityUriForResetAction(computerSystem);
            log.info("Invoking reset action ({}) on resource [ path: {} ]", resetType, computerSystem.getUri());
            webClient.post(resourceResetUri, new ResetRequest(resetType));
        } catch (WebClientRequestException e) {
            String errorMessage = format("Resource %s reset action (%s) failed", computerSystem.getUri(), resetType);
            log.warn(errorMessage + " on [ path: {} ]", computerSystem.getUri());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void powerOn(ComputerSystem computerSystem) throws EntityOperationException {
        reset(computerSystem, ON);
    }

    @Transactional(MANDATORY)
    public void shutdownGracefully(ComputerSystem computerSystem) throws EntityOperationException {
        reset(computerSystem, GRACEFUL_SHUTDOWN);
    }

    @Transactional(MANDATORY)
    public void powerOff(ComputerSystem computerSystem) throws EntityOperationException {
        reset(computerSystem, FORCE_OFF);
    }

    private URI getResettableEntityUriForResetAction(ComputerSystem computerSystem) {
        return URI.create(computerSystem.getUri() + format(RESET_URL, computerSystem.getResetName()));
    }
}
