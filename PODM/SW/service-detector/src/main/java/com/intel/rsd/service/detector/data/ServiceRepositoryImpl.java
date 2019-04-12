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

package com.intel.rsd.service.detector.data;

import lombok.val;
import org.springframework.stereotype.Component;

import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.transaction.Transactional;

import static com.intel.rsd.service.detector.data.Service.State;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Component
public class ServiceRepositoryImpl implements ServiceRepositoryExtensions {

    @PersistenceContext
    private EntityManager entityManager;

    @Transactional(REQUIRED)
    @Override
    public boolean setTrusted(String serviceId, boolean isTrusted) {
        val service = entityManager.find(Service.class, serviceId);
        if (service == null) {
            return false;
        }

        service.setTrusted(isTrusted);
        return true;
    }

    @Transactional(REQUIRED)
    @Override
    public void setServiceState(String serviceId, State newState) {

        val service = entityManager.find(Service.class, serviceId);
        if (service != null) {
            if (service.getStatus() != newState) {
                service.setStatus(newState);
            }
        }

    }
}
