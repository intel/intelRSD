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

package com.intel.rsd.podm.filters;

import com.netflix.zuul.ZuulFilter;
import com.netflix.zuul.context.RequestContext;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.MediaType;

import javax.servlet.http.HttpServletRequest;
import java.util.List;

import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.PRE_TYPE;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.SEND_ERROR_FILTER_ORDER;
import static org.springframework.http.HttpHeaders.ACCEPT;
import static org.springframework.http.HttpStatus.NOT_ACCEPTABLE;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.MediaType.parseMediaTypes;
import static org.springframework.http.MediaType.valueOf;

@Slf4j
public class ContentTypeFilter extends ZuulFilter {
    @Override
    public String filterType() {
        return PRE_TYPE;
    }

    @Override
    public int filterOrder() {
        return SEND_ERROR_FILTER_ORDER;
    }

    @Override
    public boolean shouldFilter() {
        return true;
    }

    @Override
    public Object run() {
        RequestContext context = RequestContext.getCurrentContext();
        HttpServletRequest request = context.getRequest();
        List<MediaType> mediaTypes = getMediaTypes(request.getHeader(ACCEPT));
        if (mediaTypes.stream().noneMatch(this::isCompatibleMediaType)) {
            ErrorResponse.createResponseWithStatus(context, NOT_ACCEPTABLE);
            log.info("Invalid {} request to {}", request.getMethod(), request.getRequestURL().toString());
        }
        return null;
    }

    private List<MediaType> getMediaTypes(String headers) {
        return headers != null ? parseMediaTypes(headers) : parseMediaTypes(APPLICATION_JSON_VALUE);
    }

    private boolean isCompatibleMediaType(MediaType mediaType) {
        return mediaType.isCompatibleWith(valueOf(APPLICATION_JSON_VALUE));
    }


}
