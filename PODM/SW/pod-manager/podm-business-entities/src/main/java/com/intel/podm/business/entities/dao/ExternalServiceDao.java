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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ServiceType;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.util.List;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ExternalServiceDao extends Dao<ExternalService> {
    public ExternalService getExternalServiceByUuid(UUID uuid) {
        return repository.getSingleByProperty(ExternalService.class, ExternalService.UUID, uuid);
    }

    public List<ExternalService> getExternalServicesByServiceType(ServiceType serviceType) {
        return repository.getAllByProperty(ExternalService.class, ExternalService.SERVICE_TYPE, serviceType);
    }
}
