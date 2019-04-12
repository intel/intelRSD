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

import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;

import static com.intel.rsd.service.detector.data.Service.State;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Repository
public interface ServiceRepository extends CrudRepository<Service, String>, ServiceRepositoryExtensions {
    Iterable<Service> findAllByRemoteServiceType(RemoteServiceType remoteServiceType);
    Iterable<Service> findAllByStatus(State state);

    @Transactional(REQUIRED)
    Collection<Service> deleteByServiceUri(URI serviceUri);
}
