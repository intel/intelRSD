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

package com.intel.podm.business.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.types.Id;

import javax.inject.Inject;
import java.util.Optional;

import static com.google.common.base.Preconditions.checkArgument;
import static com.intel.podm.business.entities.base.DomainObjectsRelationsChecker.is;
import static java.util.Objects.isNull;

/**
 * Allows to verify whether given {@link ContextType} is correct
 */
public class ContextValidator {
    @Inject
    GenericDao genericDao;

    @Inject
    ContextTypeToDomainObjectMapper contextTypeToDomainObjectMapper;

    public boolean isValid(Context context) {
        checkArgument(!isNull(context), "context must not be null");

        DomainObject domainObject = getIfValid(context);
        return domainObject != null;
    }

    private DomainObject getIfValid(Context context) {
        DomainObject domainObject = tryGet(context.getType(), context.getId());

        if (domainObject == null || context.getParent() == null) {
            return domainObject;
        }

        DomainObject parent = getIfValid(context.getParent());
        return parent != null && is(domainObject).containedBy(parent)
                ? domainObject
                : null;
    }

    private DomainObject tryGet(ContextType type, Id id) {
        Class domainObjectClass = contextTypeToDomainObjectMapper.get(type);

        Optional<DomainObject> domainObject = genericDao.tryFind(domainObjectClass, id);
        return domainObject.orElse(null);
    }
}
