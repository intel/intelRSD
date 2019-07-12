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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.function.Predicate;

import static java.util.Arrays.asList;

@Component
public class ModelConsistencyChecker {

    private GenericDao genericDao;

    private Collection<Checker<? extends DiscoverableEntity>> checkers = asList(
        new Checker<>(Volume.class, volume -> volume.getStorageService() != null, "Volume has no reference to StorageService")
    );

    @Autowired
    public ModelConsistencyChecker(GenericDao genericDao) {
        this.genericDao = genericDao;
    }

    public void check() {
        for (Checker<? extends DiscoverableEntity> checker : checkers) {
            checker.check();
        }
    }

    class Checker<T extends DiscoverableEntity> {
        private final Class<T> clazz;
        private final Predicate<T> predicate;
        private final String errorMessage;

        Checker(Class<T> clazz, Predicate<T> predicate, String errorMessage) {
            this.clazz = clazz;
            this.predicate = predicate;
            this.errorMessage = errorMessage;
        }

        void check() {
            for (T discoverableEntity : genericDao.findAll(clazz)) {
                if (predicate.negate().test(discoverableEntity)) {
                    throw new RuntimeException("Discovered data is inconsistent: " + errorMessage + ", entity: " + discoverableEntity);
                }
            }
        }
    }
}
