/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.StorageController;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.entities.redfish.StorageController.GET_STORAGE_CONTROLLER_MULTI_SOURCE;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class StorageControllerDao extends Dao<StorageController> {
    @Transactional(MANDATORY)
    public List<StorageController> findComplementaryStorageControllers(StorageController storageController) {
        TypedQuery<StorageController> query = entityManager.createNamedQuery(GET_STORAGE_CONTROLLER_MULTI_SOURCE, StorageController.class);
        query.setParameter("multiSourceDiscriminator", storageController.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", true);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public Optional<StorageController> findPrimaryStorageController(StorageController complementaryStorageController) {
        TypedQuery<StorageController> query = entityManager.createNamedQuery(GET_STORAGE_CONTROLLER_MULTI_SOURCE, StorageController.class);
        query.setParameter("multiSourceDiscriminator", complementaryStorageController.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", false);
        return ofNullable(singleOrNull(query.getResultList()));
    }
}
