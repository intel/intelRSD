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

package com.intel.rsd.nodecomposer.rest;

import com.intel.rsd.nodecomposer.rest.HttpServletRequestValidator.HttpServletRequestValidationException;
import com.intel.rsd.redfish.RedfishError;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import org.springframework.web.servlet.handler.HandlerInterceptorAdapter;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;

import static com.intel.rsd.json.JsonUtils.createStringWith;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;
import static org.springframework.http.MediaType.APPLICATION_JSON_UTF8_VALUE;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class RequestValidationHandlerInterceptor extends HandlerInterceptorAdapter {
    @Override
    public boolean preHandle(HttpServletRequest request, HttpServletResponse response, Object handler) throws Exception {
        HttpServletRequestValidator requestValidator = new HttpServletRequestValidator();

        try {
            requestValidator.validateServletRequest(request);
            return super.preHandle(request, response, handler);
        } catch (HttpServletRequestValidationException e) {
            setErrorResponse(response, e.getRedfishError());
            return false;
        }
    }

    private void setErrorResponse(HttpServletResponse httpServletResponse, RedfishError redfishError) {
        httpServletResponse.reset();
        httpServletResponse.resetBuffer();

        httpServletResponse.setContentType(APPLICATION_JSON_UTF8_VALUE);
        httpServletResponse.setStatus(redfishError.getHttpStatus().value());

        try (PrintWriter responseWriter = httpServletResponse.getWriter()) {
            responseWriter.print(createStringWith(redfishError));
            responseWriter.flush();
        } catch (IOException e) {
            log.error("Unable to write response. Sending empty body response.", e);
        }
    }
}
