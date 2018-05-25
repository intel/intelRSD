/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.aggregation;

import com.intel.podm.business.dto.SimpleStorageDto;
import com.intel.podm.business.entities.dao.SimpleStorageDao;
import com.intel.podm.business.entities.redfish.SimpleStorage;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;

@Dependent
public class SimpleStorageMerger extends DiscoverableEntityDataMerger<SimpleStorage, SimpleStorageDto> {
    @Inject
    private SimpleStorageDao simpleStorageDao;

    @Override
    protected List<SimpleStorage> getMultiSourceRepresentations(SimpleStorage entity) {
        return simpleStorageDao.findComplementarySimpleStorages(entity);
    }
}
