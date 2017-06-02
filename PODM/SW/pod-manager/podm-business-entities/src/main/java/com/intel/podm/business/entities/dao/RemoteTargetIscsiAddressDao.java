/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;

import javax.enterprise.context.Dependent;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class RemoteTargetIscsiAddressDao extends Dao<RemoteTargetIscsiAddress> {
    public List<RemoteTargetIscsiAddress> getRemoteTargetIscsiAddressesByTargetIqn(String targetIqn) {
        TypedQuery<RemoteTargetIscsiAddress> query = entityManager.createNamedQuery(
            RemoteTargetIscsiAddress.GET_REMOTE_TARGET_ISCSI_ADDRESSES_BY_TARGET_IQN, RemoteTargetIscsiAddress.class);
        query.setParameter("targetIqn", targetIqn);
        return query.getResultList();
    }

    public RemoteTargetIscsiAddress getRemoteTargetIscsiAddressByTargetIqn(String targetIqn) throws NonUniqueResultException {
        return singleEntityOrNull(getRemoteTargetIscsiAddressesByTargetIqn(targetIqn));
    }
}
