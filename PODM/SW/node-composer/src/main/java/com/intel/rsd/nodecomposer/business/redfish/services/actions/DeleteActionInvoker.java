/*
 * Copyright (c) 2017-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class DeleteActionInvoker {
    private final GenericDao genericDao;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public DeleteActionInvoker(GenericDao genericDao, WebClientBuilder webClientBuilder) {
        this.genericDao = genericDao;
        this.webClientBuilder = webClientBuilder;
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> void delete(T entity) throws EntityOperationException {
        requiresNonNull(entity, "entity");
        URI entityUri = entity.getUri().toUri();

        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().build()) {
            webClient.delete(entityUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected resource could not be deleted";
            log.error(format("%s on [ path: %s ]", errorMessage, entityUri), e);
            throw new EntityOperationException(errorMessage, e);
        }
        genericDao.remove(entity);
    }
}
