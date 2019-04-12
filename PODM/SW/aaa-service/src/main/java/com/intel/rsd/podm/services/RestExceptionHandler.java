/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.services;

import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.context.request.WebRequest;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

import javax.validation.ValidationException;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.UNAUTHORIZED;
import static org.springframework.http.ResponseEntity.status;

@RestControllerAdvice
@Slf4j
public class RestExceptionHandler extends ResponseEntityExceptionHandler {

    @ExceptionHandler({AccessVerificationException.class, SessionCreationException.class})
    @ResponseStatus(UNAUTHORIZED)
    @ResponseBody
    public ResponseEntity accessDenied() {
        val redfishError = createRedfishError(UNAUTHORIZED);
        return status(UNAUTHORIZED).body(redfishError);
    }

    @ExceptionHandler({ResourceNotFoundException.class})
    @ResponseStatus(NOT_FOUND)
    @ResponseBody
    public ResponseEntity resourceNotFound() {
        val redfishError = createRedfishError(NOT_FOUND);
        return status(NOT_FOUND).body(redfishError);
    }

    @ExceptionHandler({IllegalArgumentException.class})
    @ResponseStatus(BAD_REQUEST)
    @ResponseBody
    public ResponseEntity badRequest() {
        val redfishError = createRedfishError(BAD_REQUEST);
        return status(BAD_REQUEST).body(redfishError);
    }

    @ExceptionHandler({ValidationException.class})
    @ResponseStatus(BAD_REQUEST)
    @ResponseBody
    public ResponseEntity badRequest(ValidationException ex) {
        val redfishError = createRedfishError(BAD_REQUEST, ex.getMessage());
        return status(BAD_REQUEST).body(redfishError);
    }

    @ExceptionHandler({DataIntegrityException.class})
    @ResponseStatus(CONFLICT)
    @ResponseBody
    public ResponseEntity dataIntegrity() {
        val redfishError = createRedfishError(CONFLICT);
        return status(CONFLICT).body(redfishError);
    }

    @Override
    @SuppressWarnings("unchecked")
    protected ResponseEntity<Object> handleMethodArgumentNotValid(MethodArgumentNotValidException ex,
                                                                  HttpHeaders headers, HttpStatus status, WebRequest request) {
        val redfishError = createRedfishError(status);
        return ResponseEntity.badRequest().body(redfishError);
    }
}
