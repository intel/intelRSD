/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class DeleteActionInvoker {

    @Inject
    private Logger logger;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> void delete(T entity) throws EntityOperationException {
        requiresNonNull(entity, "entity");
        ExternalService service = entity.getService();
        requiresNonNull(service, () -> new IllegalStateException("There is no Service associated with selected resource"));
        URI entityUri = entity.getSourceUri();
        URI baseUri = service.getBaseUri();

        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            webClient.delete(entityUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected resource could not be deleted";
            logger.e(format("%s on [ service: %s, path: %s ]", errorMessage, baseUri, entityUri), e);
            throw new EntityOperationException(errorMessage, e);
        }
        discoverableEntityDao.removeWithConnectedExternalLinks(entity);
    }
}
