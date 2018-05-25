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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ENDPOINT;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class EndpointServiceImpl implements ReaderService<EndpointDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context fabricContext) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(fabricContext);
        List<Context> contexts = fabric.getEndpoints().stream().map(Contexts::toContext).sorted().collect(toList());
        return new CollectionDto(ENDPOINT, contexts);
    }

    @Transactional(REQUIRED)
    @Override
    public EndpointDto getResource(Context context) throws ContextResolvingException {
        Endpoint endpoint = (Endpoint) traverser.traverse(context);
        return (EndpointDto) entityToDtoMapper.map(endpoint);
    }
}
