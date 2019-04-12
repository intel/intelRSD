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

package com.intel.rsd.resourcemanager.runner;

import com.fasterxml.jackson.core.JsonParseException;
import com.intel.rsd.redfish.RedfishError;
import com.intel.rsd.resourcemanager.layers.ResourceNotFound;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestControllerAdvice;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.ResponseEntity.status;

@RestControllerAdvice
public class RestExceptionHandler {

    @ExceptionHandler({ResourceNotFound.class})
    @ResponseStatus(NOT_FOUND)
    @ResponseBody
    public ResponseEntity<RedfishError> notFound() {
        val redfishError = createRedfishError(NOT_FOUND);
        return status(redfishError.getHttpStatus()).body(redfishError);
    }

    @ExceptionHandler({JsonParseException.class})
    @ResponseStatus(BAD_REQUEST)
    @ResponseBody
    public ResponseEntity<RedfishError> jsonParseException() {
        val redfishError = createRedfishError(BAD_REQUEST, "Invalid json message");
        return status(redfishError.getHttpStatus()).body(redfishError);
    }
}
