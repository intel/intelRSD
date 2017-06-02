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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.UriConversionException;
import com.intel.podm.business.services.context.UriToContextConverter;
import com.intel.podm.business.services.redfish.CreationService;
import com.intel.podm.business.services.redfish.RemovalService;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.EventSubscriptionRequest;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.TimeoutException;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.EVENT_SUBSCRIPTION;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class SubscriptionActionServiceImpl implements CreationService<EventSubscriptionRequest>, RemovalService<EventSubscriptionRequest> {

    private final GenericDao genericDao;

    @Inject
    public SubscriptionActionServiceImpl(GenericDao genericDao) {
        requiresNonNull(genericDao, "genericDao");
        this.genericDao = genericDao;
    }

    @Override
    @Transactional(REQUIRED)
    public void perform(Context context) throws BusinessApiException, TimeoutException {
        try {
            EventSubscription subscription = genericDao.find(EventSubscription.class, context.getId());
            genericDao.remove(subscription);
        } catch (EntityNotFoundException e) {
            throw new ContextResolvingException(e.getMessage(), context, e);
        }
    }

    @Override
    @Transactional(REQUIRED)
    public Context create(Context context, EventSubscriptionRequest request) throws BusinessApiException, TimeoutException {
        SubscriptionRequestValidator validator = new SubscriptionRequestValidator();
        EventSubscription eventSubscription = createSubscriptionEntity(validator.validate(request));
        // TODO: this is a hack!
        Context subscriptionContext = contextOf(id(""), EVENT_SERVICE);
        return subscriptionContext.child(eventSubscription.getId(), EVENT_SUBSCRIPTION);
    }

    private EventSubscription createSubscriptionEntity(final EventSubscriptionRequest request) {
        EventSubscription eventSubscription = genericDao.create(EventSubscription.class);

        eventSubscription.setName(request.getName());
        eventSubscription.setProtocol(request.getProtocol());
        eventSubscription.setSubscriptionContext(request.getSubscriptionContext());
        eventSubscription.setDescription(request.getDescription());
        eventSubscription.setDestination(request.getDestination().toString());
        request.getEventTypes().forEach(eventSubscription::addEventType);
        if (request.getOriginResources() != null) {
            request.getOriginResources().stream()
                .map(ODataId::toString)
                .forEach(eventSubscription::addOriginResource);
        }

        return eventSubscription;
    }

    static class SubscriptionRequestValidator {
        private EventSubscriptionRequest validate(final EventSubscriptionRequest request) throws RequestValidationException {
            Violations violations = new Violations();
            if (isNullOrEmpty(request.getSubscriptionContext())) {
                violations.addViolation("Context is either null or empty.");
            }
            if (!Objects.equals(request.getProtocol(), "Redfish")) {
                violations.addViolation("Protocol is other than Redfish.");
            }
            if (request.getEventTypes() == null || request.getEventTypes().isEmpty()) {
                violations.addViolation("EventTypes is either null or empty.");
            }

            violations.addAll(originResourcesViolations(request.getOriginResources()));
            violations.addAll(destinationViolations(request.getDestination()));

            if (violations.hasViolations()) {
                throw new RequestValidationException("Received event subscription creation request is wrong", violations, null);
            }

            return request;
        }

        private Violations originResourcesViolations(List<ODataId> originResources) {
            Violations violations = new Violations();
            for (ODataId originResource : originResources) {
                try {
                    UriToContextConverter.getContextFromUri(originResource.toUri());
                } catch (UriConversionException e) {
                    violations.addViolation(e.getMessage());
                }
            }
            return violations;
        }

        Violations destinationViolations(URI destination) {
            Violations violations = new Violations();
            if (destination == null) {
                violations.addMissingPropertyViolation("Destination");
                return violations;
            }

            if (!destination.isAbsolute()) {
                violations.addViolation("Destination is not absolute URI.");
            }

            String scheme = destination.getScheme();
            if (scheme == null) {
                violations.addViolation("Lack of Destination's protocol - must be either 'http' or 'https'.");
                return violations;
            }

            if (!Arrays.asList("http", "https").contains(scheme)) {
                violations.addViolation("Destination's protocol must be either 'http' or 'https'");
            }

            return violations;
        }
    }
}
