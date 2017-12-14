/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.dto.EventSubscriptionDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.EVENT_SUBSCRIPTION;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class EventSubscriptionServiceImpl implements ReaderService<EventSubscriptionDto> {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context eventServiceContext) throws ContextResolvingException {
        return new CollectionDto(
            EVENT_SUBSCRIPTION,
            genericDao.findAll(EventSubscription.class).stream()
                .map(EventSubscription::getId)
                .collect(toSet())
        );
    }

    @Transactional(REQUIRED)
    @Override
    public EventSubscriptionDto getResource(Context context) throws ContextResolvingException {
        try {
            EventSubscription eventSubscription = genericDao.find(EventSubscription.class, context.getId());
            return (EventSubscriptionDto) entityToDtoMapper.map(eventSubscription);
        } catch (EntityNotFoundException e) {
            throw new ContextResolvingException(e.getMessage(), context, e);
        }
    }
}
