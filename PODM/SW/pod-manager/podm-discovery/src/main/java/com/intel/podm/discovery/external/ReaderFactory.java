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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.client.api.reader.ExternalServiceReaderFactory;
import com.intel.podm.common.types.ServiceType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.EnumSet;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.lang.String.format;
import static java.util.EnumSet.of;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Transactional(REQUIRES_NEW)
public class ReaderFactory {
    private static final EnumSet<ServiceType> SUPPORTED_SERVICE_TYPES = of(LUI, PSME, RSS, RMM);

    @Inject
    private ExternalServiceRepository repository;

    @Inject
    private ExternalServiceReaderFactory externalServiceReaderFactory;

    public ExternalServiceReader create(UUID serviceUuid) {
        ExternalService service = repository.find(serviceUuid);
        URI baseUri = service.getBaseUri();
        ServiceType serviceType = service.getServiceType();

        if (SUPPORTED_SERVICE_TYPES.contains(serviceType)) {
            return externalServiceReaderFactory.createExternalServiceReader(baseUri);
        } else {
            throw new UnsupportedOperationException(format("Service type '%s' is not supported", serviceType));
        }
    }
}
