/*
 * Copyright (c) 2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.PcieDriveErasedUpdateRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.ProcessorErasedUpdateRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;

import static java.net.URI.create;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class SecureEraseInvoker<T extends DiscoverableEntity, Y extends ExternalServiceResource> {
    private static final String PCIE_DRIVE_SECURE_ERASE_PATH_PART = "/Actions/Drive.SecureErase";
    private static final String PROCESSOR_SECURE_ERASE_PATH_PART = "/Actions/Oem/Intel.Oem.SecureErase";

    private final WebClientBuilder webClientBuilder;

    public SecureEraseInvoker(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    public void secureErase(T entity) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            webClient.post(getSecureEraseUri(entity), null);
        } catch (WebClientRequestException e) {
            String errorMessage = "SecureErase action failed";
            log.warn(errorMessage + " on [{}: {}, details: {}]",
                entity.getClass().getSimpleName(),
                entity.getUri(),
                e.getMessage());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    public void updateErased(T entity, boolean erased) throws EntityOperationException {
        performUpdateErasedAction(entity, erased);
    }

    private Y performUpdateErasedAction(T entity, boolean erased) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return patchEntityAndRetrieve(entity, erased, webClient);
        } catch (WebClientRequestException e) {
            String errorMessage = "Update Erased flag action failed";
            log.warn(errorMessage + " on [{}: {}, details: {}]",
                entity.getClass().getSimpleName(),
                entity.getUri(),
                e.getMessage());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private Y patchEntityAndRetrieve(T entity, boolean erased, WebClient webClient) throws WebClientRequestException {
        if (isProcessor(entity)) {
            return webClient.patchAndRetrieve(entity.getUri().toUri(), new ProcessorErasedUpdateRequest(erased));
        }

        return webClient.patchAndRetrieve(entity.getUri().toUri(), new PcieDriveErasedUpdateRequest(erased));
    }

    private URI getSecureEraseUri(T entity) {
        ODataId prefix = entity.getUri();
        if (isProcessor(entity)) {
            return create(prefix + PROCESSOR_SECURE_ERASE_PATH_PART);
        }

        return create(prefix + PCIE_DRIVE_SECURE_ERASE_PATH_PART);
    }

    private boolean isProcessor(T entity) {
        return entity instanceof Processor;
    }
}
