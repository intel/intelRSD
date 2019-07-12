/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

@Component
public class EntityTreeTraverser {
    private final GenericDao genericDao;

    @Autowired
    public EntityTreeTraverser(GenericDao genericDao) {
        this.genericDao = genericDao;
    }

    public DiscoverableEntity traverseDiscoverableEntity(ODataId uri) throws ODataIdResolvingException {
        requiresNonNull(uri, "uri");

        DiscoverableEntity discoverableEntity = tryGetDiscoverableEntity(uri);
        if (discoverableEntity == null) {
            throw new ODataIdResolvingException(uri);
        }

        return discoverableEntity;
    }

    public ComposedNode traverseComposedNode(ODataId uri) throws ODataIdResolvingException {
        requiresNonNull(uri, "uri");

        ComposedNode composedNode = tryGetComposedNode(uri);
        if (composedNode == null) {
            throw new ODataIdResolvingException(uri);
        }

        return composedNode;
    }

    public DiscoverableEntity tryGetDiscoverableEntity(ODataId uri) {
        return genericDao.tryFind(DiscoverableEntity.class, uri).orElse(null);
    }

    public ComposedNode tryGetComposedNode(ODataId uri) {
        return genericDao.tryFind(ComposedNode.class, uri).orElse(null);
    }
}
