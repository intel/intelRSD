/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.error;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.exc.UnrecognizedPropertyException;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.ComposedNodeAssetNotAvailableException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.types.exceptions.NoRetriesLeftException;
import com.intel.rsd.redfish.RedfishError;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.HttpMessageNotReadableException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.context.request.WebRequest;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

import javax.persistence.NoResultException;
import javax.persistence.PersistenceException;
import javax.validation.ConstraintViolation;
import javax.validation.ConstraintViolationException;
import java.util.Collection;
import java.util.Comparator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.TreeSet;
import java.util.concurrent.TimeoutException;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishErrorWithCustomMessage;
import static java.lang.String.format;
import static java.lang.String.valueOf;
import static java.util.Collections.unmodifiableList;
import static java.util.Optional.ofNullable;
import static org.apache.commons.lang3.exception.ExceptionUtils.getThrowableList;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.NOT_IMPLEMENTED;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.springframework.http.ResponseEntity.status;

@Slf4j
@RestControllerAdvice
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class RestExceptionHandler extends ResponseEntityExceptionHandler {

    private final ExternalServiceErrorResponseBuilder externalServiceErrorResponseBuilder;
    private final List<Class> exceptions = unmodifiableList(newArrayList(UnrecognizedPropertyException.class, JsonMappingException.class));

    @Autowired
    public RestExceptionHandler(ExternalServiceErrorResponseBuilder externalServiceErrorResponseBuilder) {
        this.externalServiceErrorResponseBuilder = externalServiceErrorResponseBuilder;
    }

    @ExceptionHandler(ComposedNodeAssetNotAvailableException.class)
    @ResponseBody
    @ResponseStatus(SERVICE_UNAVAILABLE)
    public ResponseEntity<RedfishError> composedNodeAssetNotAvailableException(ComposedNodeAssetNotAvailableException ex) {
        val redfishError = createRedfishErrorWithCustomMessage(SERVICE_UNAVAILABLE, format("Retry request after %s seconds.", ex.getRetryAfterSeconds()));
        log.error("{}", redfishError.getError());
        log.debug(ex.getMessage(), ex);
        return status(SERVICE_UNAVAILABLE).header("Retry-After", valueOf(ex.getRetryAfterSeconds())).body(redfishError);
    }

    @ExceptionHandler({BusinessApiException.class})
    @ResponseBody
    @ResponseStatus(INTERNAL_SERVER_ERROR)
    public ResponseEntity<RedfishError> businessApiException(BusinessApiException ex) {
        Optional<ResponseEntity<RedfishError>> externalServiceErrorInExceptionStack = externalServiceErrorResponseBuilder.getExternalServiceErrorResponse(ex);
        if (externalServiceErrorInExceptionStack.isPresent()) {
            return externalServiceErrorInExceptionStack.get();
        }

        log.error(format("BusinessApiException: %s", ex.getMessage()), ex);
        val redfishError = createRedfishError(INTERNAL_SERVER_ERROR);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({ConstraintViolationException.class})
    @ResponseBody
    @ResponseStatus(BAD_REQUEST)
    public ResponseEntity<RedfishError> constraintViolationException(ConstraintViolationException ex) {
        // TODO: RSASW-8926 - Consider removing 'dirty fix' for double error message in response
        val extendedInfo = removeDuplicatedConstraintViolations(ex).stream()
            .map(ConstraintViolation::getMessage)
            .filter(Objects::nonNull)
            .toArray(String[]::new);

        val redfishError = createRedfishError(BAD_REQUEST, extendedInfo);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({EntityOperationException.class})
    @ResponseBody
    @ResponseStatus(INTERNAL_SERVER_ERROR)
    public ResponseEntity<RedfishError> entityOperationException(EntityOperationException ex) {
        Optional<ResponseEntity<RedfishError>> errorResponse = externalServiceErrorResponseBuilder.getExternalServiceErrorResponse(ex);
        if (errorResponse.isPresent()) {
            return errorResponse.get();
        }

        log.error(ex.getMessage(), ex);
        val redfishError = createRedfishErrorWithCustomMessage(
            INTERNAL_SERVER_ERROR,
            format("%s exception encountered.", EntityOperationException.class.getSimpleName())
        );
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({JsonProcessingException.class})
    @ResponseBody
    @ResponseStatus(BAD_REQUEST)
    public ResponseEntity<RedfishError> jsonProcessingException(JsonProcessingException ex) {
        log.error("JSON Processing error", ex);
        val redfishError = createRedfishError(BAD_REQUEST);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({NodeComposerWebException.class})
    @ResponseBody
    public ResponseEntity<RedfishError> nodeComposerWebException(NodeComposerWebException ex) {
        return status(ex.getStatusCode()).body(ex.getRedfishError());
    }

    @ExceptionHandler({NoResultException.class})
    @ResponseBody
    @ResponseStatus(INTERNAL_SERVER_ERROR)
    public ResponseEntity<RedfishError> noResultException(NoResultException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(INTERNAL_SERVER_ERROR);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({NoRetriesLeftException.class})
    @ResponseBody
    @ResponseStatus(SERVICE_UNAVAILABLE)
    public ResponseEntity<RedfishError> noRetriesLeftException(NoRetriesLeftException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(SERVICE_UNAVAILABLE);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({ODataIdResolvingException.class})
    @ResponseBody
    @ResponseStatus(NOT_FOUND)
    public ResponseEntity<RedfishError> oDataIdResolvingException(ODataIdResolvingException ex) {
        val extendedInfo = ofNullable(ex.getODataId())
            .map(odata -> format("Provided URI %s could not be resolved", odata))
            .orElseGet(ex::getMessage);

        log.warn(extendedInfo);
        log.debug(extendedInfo, ex);
        val redfishError = createRedfishError(NOT_FOUND, extendedInfo);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({PersistenceException.class})
    @ResponseBody
    @ResponseStatus(SERVICE_UNAVAILABLE)
    public ResponseEntity<RedfishError> persistenceException(PersistenceException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(SERVICE_UNAVAILABLE);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({RequestValidationException.class})
    @ResponseBody
    @ResponseStatus(BAD_REQUEST)
    public ResponseEntity<RedfishError> requestValidationException(RequestValidationException ex) {
        log.error(ex.getMessage());
        log.trace(ex.getMessage(), ex);
        val redfishError = createRedfishErrorWithCustomMessage(BAD_REQUEST, ex.getMessage(), ex.getViolations().asStringList().toArray(new String[0]));
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({ResourceStateMismatchException.class})
    @ResponseBody
    @ResponseStatus(CONFLICT)
    public ResponseEntity<RedfishError> resourceStateMismatchException(ResourceStateMismatchException ex) {
        val redfishError = createRedfishError(
            CONFLICT,
            format("%s exception encountered.", ResourceStateMismatchException.class.getSimpleName()),
            ex.getMessage());

        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({TimeoutException.class})
    @ResponseBody
    @ResponseStatus(SERVICE_UNAVAILABLE)
    public ResponseEntity<RedfishError> timeoutException(TimeoutException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(SERVICE_UNAVAILABLE);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({UnsupportedOperationException.class})
    @ResponseBody
    @ResponseStatus(NOT_IMPLEMENTED)
    public ResponseEntity<RedfishError> unsupportedOperationException(UnsupportedOperationException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(NOT_IMPLEMENTED);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({IllegalArgumentException.class})
    @ResponseBody
    @ResponseStatus(BAD_REQUEST)
    public ResponseEntity<RedfishError> illegalArgumentException(RuntimeException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(BAD_REQUEST);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({RuntimeException.class})
    @ResponseBody
    @ResponseStatus(INTERNAL_SERVER_ERROR)
    public ResponseEntity<RedfishError> runtimeException(RuntimeException ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(INTERNAL_SERVER_ERROR);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({Exception.class})
    @ResponseBody
    @ResponseStatus(INTERNAL_SERVER_ERROR)
    public ResponseEntity<RedfishError> exception(Exception ex) {
        log.error("Application Error: {}", ex.getMessage(), ex);
        val redfishError = createRedfishError(INTERNAL_SERVER_ERROR);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @Override
    protected ResponseEntity<Object> handleHttpMessageNotReadable(HttpMessageNotReadableException ex, HttpHeaders headers, HttpStatus status,
                                                                  WebRequest request) {
        if (isRedfishResponseMandatory(ex)) {
            return status(status).body(createRedfishError(status));
        }

        return super.handleHttpMessageNotReadable(ex, headers, status, request);
    }

    private Collection<ConstraintViolation> removeDuplicatedConstraintViolations(ConstraintViolationException exception) {
        final Collection<ConstraintViolation> constraintViolations = new TreeSet<>(Comparator.comparing(ConstraintViolation::getMessage));
        constraintViolations.addAll(exception.getConstraintViolations());
        return constraintViolations;
    }

    private boolean isRedfishResponseMandatory(Throwable e) {
        return getThrowableList(e).stream()
            .anyMatch(throwable -> exceptions.contains(throwable.getClass()));
    }
}
