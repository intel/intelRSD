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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.services.context.Context;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;


/**
 * Allows to traverse path (expressed by {@link Context}) of {@link DomainObject} tree.
 * Throws exception if {@link DomainObject} is unreachable.
 */
@Transactional(MANDATORY)
public class DomainObjectTreeTraverser {
    @Inject
    GenericDao genericDao;

    @Inject
    ContextValidator validator;

    @Inject
    ContextTypeToDomainObjectMapper mapper;

    public DomainObject traverse(Context context) throws EntityNotFoundException {
        if (context == null) {
            throw new IllegalArgumentException("context must not be null");
        }

        if (!validator.isValid(context)) {
            throw new EntityNotFoundException("context is not valid");
        }

        Class<? extends DomainObject> domainObjectClass = mapper.get(context.getType());
        return genericDao.find(domainObjectClass, context.getId());
    }

    public <T extends DomainObject> List<T> traverse(List<Context> contexts) throws EntityNotFoundException {
        if (contexts == null) {
            throw new IllegalArgumentException("List of contexts must not be null");
        }

        List<T> domainObjects = new ArrayList<>();

        for (Context context : contexts) {
            domainObjects.add((T) traverse(context));
        }

        return domainObjects;
    }
}
