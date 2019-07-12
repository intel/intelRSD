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

package com.intel.rsd.podm.filters;

import com.netflix.zuul.ZuulFilter;
import com.netflix.zuul.context.RequestContext;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpMethod;

import javax.servlet.http.HttpServletRequest;

import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.PRE_TYPE;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.SERVLET_DETECTION_FILTER_ORDER;
import static org.springframework.http.HttpStatus.NOT_IMPLEMENTED;

@Slf4j
public class HttpMethodFilter extends ZuulFilter {
    @Override
    public String filterType() {
        return PRE_TYPE;
    }

    // filteredOrder should be before AccessVerifierFilter
    @Override
    public int filterOrder() {
        return SERVLET_DETECTION_FILTER_ORDER - 2;
    }

    @Override
    public boolean shouldFilter() {
        return true;
    }

    @Override
    public Object run() {
        RequestContext context = RequestContext.getCurrentContext();
        HttpServletRequest request = context.getRequest();
        try {
            HttpMethod.valueOf(request.getMethod());
        } catch (IllegalArgumentException e) {
            log.warn("Not able to recognize HttpRequest: {}", request.getMethod());
            ErrorResponse.createResponseWithStatus(context, NOT_IMPLEMENTED);
        }
        return null;
    }
}
