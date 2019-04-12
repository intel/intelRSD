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

package com.intel.rsd.service.detector.endpoint;

import com.fasterxml.jackson.databind.exc.InvalidFormatException;
import com.intel.rsd.redfish.RedfishError;
import com.intel.rsd.service.detector.endpoint.registration.ServiceDefinitionException;
import com.intel.rsd.service.detector.endpoint.registration.ServiceDescribingException;
import com.intel.rsd.service.detector.endpoint.registration.ServicePersistingException;
import com.intel.rsd.service.detector.endpoint.registration.ServiceStateConflictException;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.HttpMessageNotReadableException;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.context.request.WebRequest;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.springframework.http.ResponseEntity.badRequest;

@RestControllerAdvice
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class RestExceptionHandler extends ResponseEntityExceptionHandler {
    @ExceptionHandler({ServiceDefinitionException.class})
    @ResponseStatus(BAD_REQUEST)
    @ResponseBody
    public RedfishError errorWhileRegisteringService(ServiceDefinitionException ex) {
        return createRedfishError(BAD_REQUEST, ex.getMessage());
    }

    @ExceptionHandler({ServiceDescribingException.class})
    @ResponseStatus(SERVICE_UNAVAILABLE)
    @ResponseBody
    public RedfishError errorWhileDescribingService(ServiceDescribingException ex) {
        return createRedfishError(SERVICE_UNAVAILABLE, ex.getMessage());
    }

    @ExceptionHandler({ServiceStateConflictException.class})
    @ResponseBody
    @ResponseStatus(CONFLICT)
    public RedfishError errorWhilePersistingService(ServiceStateConflictException ex) {
        return createRedfishError(CONFLICT, ex.getMessage());
    }

    @ExceptionHandler({ServicePersistingException.class})
    @ResponseBody
    @ResponseStatus(CONFLICT)
    public RedfishError errorWhilePersistingService(ServicePersistingException ex) {
        return createRedfishError(CONFLICT, ex.getMessage());
    }

    @Override
    protected ResponseEntity<Object> handleHttpMessageNotReadable(HttpMessageNotReadableException ex, HttpHeaders headers,
                                                                  HttpStatus status, WebRequest request) {
        Throwable specificCause = ex.getMostSpecificCause();
        if (specificCause instanceof InvalidFormatException) {
            return badRequest().body(createRedfishError(BAD_REQUEST, ((InvalidFormatException) specificCause).getOriginalMessage()));
        } else {
            return badRequest().body(createRedfishError(BAD_REQUEST));
        }
    }

    @Override
    protected ResponseEntity<Object> handleMethodArgumentNotValid(MethodArgumentNotValidException ex, HttpHeaders headers,
                                                                  HttpStatus status, WebRequest request) {
        return badRequest().body(createRedfishError(BAD_REQUEST, ex.getBindingResult()));
    }

    @Override
    protected ResponseEntity<Object> handleExceptionInternal(Exception ex, Object body, HttpHeaders headers, HttpStatus status, WebRequest request) {
        return super.handleExceptionInternal(ex, createRedfishError(status), headers, status, request);
    }
}
