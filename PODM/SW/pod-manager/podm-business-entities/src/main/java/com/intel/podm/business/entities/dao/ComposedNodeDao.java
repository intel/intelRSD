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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.ComposedNode.GET_ALL_NODES_IDS;
import static com.intel.podm.business.entities.redfish.ComposedNode.GET_NODE_BY_ASSOCIATED_COMPUTER_SYSTEM_UUID;
import static com.intel.podm.business.entities.redfish.ComposedNode.GET_NODES_ELIGIBLE_FOR_RECOVERY;
import static com.intel.podm.common.utils.IterableHelper.optionalSingle;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class ComposedNodeDao extends Dao<ComposedNode> {
    @Transactional(MANDATORY)
    public Optional<ComposedNode> getComposedNodeByAssociatedSystemUuid(UUID uuid) {
        TypedQuery<ComposedNode> query = entityManager.createNamedQuery(GET_NODE_BY_ASSOCIATED_COMPUTER_SYSTEM_UUID, ComposedNode.class);
        query.setParameter("uuid", uuid);

        return optionalSingle(query.getResultList());
    }

    @Transactional(MANDATORY)
    public List<ComposedNode> getComposedNodesEligibleForRecovery() {
        return entityManager.createNamedQuery(GET_NODES_ELIGIBLE_FOR_RECOVERY, ComposedNode.class).getResultList();
    }

    @Transactional(MANDATORY)
    public List<Id> getAllComposedNodeIds() {
        return entityManager.createNamedQuery(GET_ALL_NODES_IDS, Id.class).getResultList();
    }
}
