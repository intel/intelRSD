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

package com.intel.podm.eventing;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.eventing.dto.EventArrayDto;
import com.intel.podm.eventing.handlers.EventHandlerFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class EventListenerService {

    @Inject
    private Logger logger;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private EventHandlerFactory eventHandlerFactory;

    @Transactional(REQUIRED)
    public void dispatch(EventArrayDto eventArrayDto) throws EventException {
        UUID serviceUuid = eventArrayDto.getServiceUuid();
        ExternalService service = externalServiceDao.getExternalServiceByUuid(serviceUuid);
        if (service != null) {
            eventHandlerFactory.createEventHandler(service.getServiceType()).handle(eventArrayDto);
        } else {
            logger.d("Service with UUID: {} does not exist", serviceUuid);
            throw new EventException();
        }
    }
}
