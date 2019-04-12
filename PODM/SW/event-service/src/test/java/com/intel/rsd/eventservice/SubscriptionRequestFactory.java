/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.eventservice;

import com.intel.rsd.dto.ODataId;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.dto.events.SubscriptionRequest;

import javax.validation.ConstraintViolation;
import javax.validation.ConstraintViolationException;
import javax.validation.Validation;
import javax.validation.Validator;
import java.net.URI;
import java.util.Set;

public class SubscriptionRequestFactory {
    private static final Validator VALIDATOR = Validation.buildDefaultValidatorFactory().getValidator();

    public static SubscriptionRequest create(String name,
                                             URI destination,
                                             String context,
                                             Set<ODataId> oDataIds,
                                             Set<EventType> eventTypes) {

        SubscriptionRequest sr = new SubscriptionRequest();
        sr.setName(name);
        sr.setDestination(destination);
        sr.setEventTypes(eventTypes);
        sr.setContext(context);
        sr.setOriginResources(oDataIds);

        Set<ConstraintViolation<SubscriptionRequest>> violations = VALIDATOR.validate(sr);
        if (!violations.isEmpty()) {
            throw new ConstraintViolationException("One or more given arguments have illegal values", violations);
        }
        return sr;
    }
}
