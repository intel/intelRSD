/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;

/**
 * Allows to verify whether given {@link ContextType} is correct
 */
@Dependent
public class ContextValidator {
    @Inject
    GenericDao genericDao;

    @Inject
    ContextTypeToEntityMapper contextTypeToEntityMapper;

    public boolean isValid(Context context) {
        requiresNonNull(context, "context");

        Entity entity = getIfValid(context);
        return entity != null;
    }

    private Entity getIfValid(Context context) {
        Entity entity = tryGet(context.getType(), context.getId());

        if (entity == null || context.getParent() == null) {
            return entity;
        }

        Entity parent = getIfValid(context.getParent());
        return parent != null && entity.containedBy(parent)
            ? entity
            : null;
    }

    private Entity tryGet(ContextType type, Id id) {
        Class<? extends Entity> entityClass;
        try {
            entityClass = contextTypeToEntityMapper.getEntityClass(type);
        } catch (UnsupportedOperationException e) {
            return null;
        }
        return genericDao.tryFind(entityClass, id).orElse(null);
    }
}
