/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.entities.redfish.StorageService.GET_ALL_STORAGE_SERVICE_IDS;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class StorageServiceDao extends Dao<StorageService> {
    @Transactional(MANDATORY)
    public List<Id> getAllStorageServiceIds() {
        return entityManager.createNamedQuery(GET_ALL_STORAGE_SERVICE_IDS, Id.class).getResultList();
    }
}
