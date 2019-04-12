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

import com.fasterxml.jackson.databind.exc.InvalidFormatException;
import com.intel.rsd.redfish.RedfishError;
import lombok.val;
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
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.ResponseEntity.badRequest;

@RestControllerAdvice
public class RestExceptionHandler extends ResponseEntityExceptionHandler {
    @ExceptionHandler({NotFoundException.class})
    @ResponseStatus(NOT_FOUND)
    @ResponseBody
    public RedfishError notFoundException() {
        return createRedfishError(NOT_FOUND);
    }

    @ExceptionHandler({BadRequestException.class})
    @ResponseStatus(BAD_REQUEST)
    @ResponseBody
    public RedfishError notFoundException(BadRequestException exception) {
        return createRedfishError(BAD_REQUEST, exception.getMessage());
    }

    @Override
    protected ResponseEntity<Object> handleHttpMessageNotReadable(HttpMessageNotReadableException ex,
                                                                  HttpHeaders headers,
                                                                  HttpStatus status,
                                                                  WebRequest request) {
        val specificCause = ex.getMostSpecificCause();
        if (specificCause instanceof InvalidFormatException) {
            return badRequest().body(createRedfishError(BAD_REQUEST, ((InvalidFormatException) specificCause).getOriginalMessage()));
        } else {
            return badRequest().body(createRedfishError(BAD_REQUEST));
        }
    }

    @Override
    protected ResponseEntity<Object> handleMethodArgumentNotValid(MethodArgumentNotValidException ex,
                                                                  HttpHeaders headers,
                                                                  HttpStatus status,
                                                                  WebRequest request) {

        val redfishError = createRedfishError(BAD_REQUEST, ex.getBindingResult());
        return badRequest().body(redfishError);
    }

    @Override
    protected ResponseEntity<Object> handleExceptionInternal(Exception ex, Object body, HttpHeaders headers, HttpStatus status, WebRequest request) {
        val redfishError = createRedfishError(INTERNAL_SERVER_ERROR);
        return super.handleExceptionInternal(ex, redfishError, headers, status, request);
    }
}
