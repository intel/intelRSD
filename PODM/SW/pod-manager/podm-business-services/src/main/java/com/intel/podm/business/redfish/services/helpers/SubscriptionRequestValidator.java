/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.services.context.UriConversionException;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.EventSubscriptionRequest;

import java.net.URI;
import java.util.List;
import java.util.Objects;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;
import static java.util.Arrays.asList;

public final class SubscriptionRequestValidator {
    private SubscriptionRequestValidator() {
    }

    @SuppressWarnings({"checkstyle:NPathComplexity"})
    public static EventSubscriptionRequest validateEventSubscriptionRequest(final EventSubscriptionRequest request) throws RequestValidationException {
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

    private static Violations originResourcesViolations(List<ODataId> originResources) {
        Violations violations = new Violations();
        for (ODataId originResource : originResources) {
            try {
                getContextFromUri(originResource.toUri());
            } catch (UriConversionException e) {
                violations.addViolation(e.getMessage());
            }
        }
        return violations;
    }

    private static Violations destinationViolations(URI destination) {
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

        if (!asList("http", "https").contains(scheme)) {
            violations.addViolation("Destination's protocol must be either 'http' or 'https'");
        }

        return violations;
    }
}
