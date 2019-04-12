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

package com.intel.rsd.redfish;

import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.web.servlet.error.ErrorController;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static java.util.UUID.randomUUID;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.MediaType.APPLICATION_JSON;
import static org.springframework.http.ResponseEntity.status;

@RestController
@Slf4j
public class RedfishErrorController implements ErrorController {
    @Value("${error.path:/error}")
    private String errorPath;

    @Override
    public String getErrorPath() {
        return errorPath;
    }

    @RequestMapping(value = "${error.path:/error}", produces = "application/json")
    public ResponseEntity error(HttpServletRequest servletRequest, HttpServletResponse servletResponse) {
        val statusCode = HttpStatus.valueOf(servletResponse.getStatus());
        if (statusCode == INTERNAL_SERVER_ERROR) {
            val redfishError = createRedfishError(statusCode, "Unknown error occurred. For details consult log using " + randomUUID());
            log.error("{}", redfishError);
            return status(statusCode).contentType(APPLICATION_JSON).body(redfishError);
        } else {
            val redfishError = createRedfishError(statusCode);
            return status(statusCode).contentType(APPLICATION_JSON).body(redfishError);
        }
    }
}
