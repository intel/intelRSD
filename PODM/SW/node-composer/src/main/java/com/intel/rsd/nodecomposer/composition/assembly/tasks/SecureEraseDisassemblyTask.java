/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.MemorySecureEraseRequest;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;
import java.net.URI;

import static com.intel.rsd.nodecomposer.types.redfish.ActionsResourceNames.ACTIONS_MEMORY_SECURE_ERASE;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@Slf4j
@Component
@Scope(SCOPE_PROTOTYPE)
public class SecureEraseDisassemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 1011258564518988025L;
    private final transient ComputerSystemDao computerSystemDao;
    private final transient WebClientBuilder webClientBuilder;

    @Setter
    private ODataId computerSystemOdataId;
    @Setter
    private Boolean resetConfiguration;
    @Setter
    private Boolean eraseConfigurationKeys;

    @Autowired
    public SecureEraseDisassemblyTask(ComputerSystemDao computerSystemDao, WebClientBuilder webClientBuilder) {
        this.computerSystemDao = computerSystemDao;
        this.webClientBuilder = webClientBuilder;
    }

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() throws RuntimeException {
        ComputerSystem computerSystem = computerSystemDao.find(computerSystemOdataId);
        ODataId computerSystemOdataId = computerSystem.getUri();
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            URI secureEraseUri = fromUri(computerSystemOdataId.toUri()).pathSegment(ACTIONS_MEMORY_SECURE_ERASE).build().toUri();
            log.info(format("Invoking secure erase action (%s) on resource [ path: %s ]", computerSystemOdataId, secureEraseUri));

            webClient.post(secureEraseUri, new MemorySecureEraseRequest(resetConfiguration, eraseConfigurationKeys));
        } catch (WebClientRequestException e) {
            log.error(format("SecureErase action failed on [system: %s, details: %s]", computerSystemOdataId, e.getMessage()), e);
        }
    }
}
