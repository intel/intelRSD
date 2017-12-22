/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.rest.representation.json.exceptionmappers;

import javax.enterprise.context.ApplicationScoped;
import javax.validation.ConstraintViolation;
import javax.validation.ConstraintViolationException;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;
import java.util.Collection;
import java.util.Comparator;
import java.util.TreeSet;

import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ErrorType.INVALID_PAYLOAD;
import static java.util.stream.Collectors.toList;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@ApplicationScoped
@Provider
@Produces(APPLICATION_JSON)
public class ConstraintViolationExceptionMapper implements ExceptionMapper<ConstraintViolationException> {
    @Override
    public Response toResponse(ConstraintViolationException exception) {
        // TODO: RSASW-8926 - Consider removing 'dirty fix' for double error message in response
        final Collection<ConstraintViolation> constraintViolations = removeDuplicatedConstraintViolations(exception);

        return newErrorResponseBuilder(INVALID_PAYLOAD)
            .withDetails(constraintViolations
                .stream()
                .map(ConstraintViolation::getMessage)
                .collect(toList())
            ).build();
    }

    private Collection<ConstraintViolation> removeDuplicatedConstraintViolations(ConstraintViolationException exception) {
        final Collection<ConstraintViolation> constraintViolations = new TreeSet<>(Comparator.comparing(ConstraintViolation::getMessage));
        constraintViolations.addAll(exception.getConstraintViolations());
        return constraintViolations;
    }
}
