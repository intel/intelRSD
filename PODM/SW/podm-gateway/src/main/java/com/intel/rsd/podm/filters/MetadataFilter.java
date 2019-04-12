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

import com.intel.rsd.podm.MetadataResourceProvider;
import com.netflix.zuul.ZuulFilter;
import com.netflix.zuul.context.RequestContext;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.regex.Pattern;

import static com.netflix.zuul.context.RequestContext.getCurrentContext;
import static java.util.regex.Pattern.compile;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.ROUTE_TYPE;
import static org.springframework.cloud.netflix.zuul.filters.support.FilterConstants.SEND_FORWARD_FILTER_ORDER;
import static org.springframework.http.HttpHeaders.CONTENT_TYPE;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.MediaType.APPLICATION_XML_VALUE;

@Slf4j
@Component
public class MetadataFilter extends ZuulFilter {
    private static final Pattern URI_PATTERN = compile("^/redfish/v1/\\$?metadata.*");

    private MetadataResourceProvider metadataResourceProvider;

    @Autowired
    public MetadataFilter(MetadataResourceProvider metadataResourceProvider) {
        this.metadataResourceProvider = metadataResourceProvider;
    }

    @Override
    public String filterType() {
        return ROUTE_TYPE;
    }

    @Override
    public int filterOrder() {
        return SEND_FORWARD_FILTER_ORDER;
    }

    @Override
    public boolean shouldFilter() {
        final String requestUri = getCurrentContext().getRequest().getRequestURI();

        return URI_PATTERN.matcher(requestUri).matches();
    }

    @Override
    public Object run() {
        final RequestContext context = getCurrentContext();
        final String requestUri = context.getRequest().getRequestURI();

        String body = metadataResourceProvider.getMetadataBody(requestUri);
        if (body != null) {
            context.setResponseBody(body);
            context.addZuulResponseHeader(CONTENT_TYPE, APPLICATION_XML_VALUE);
            context.setResponseStatusCode(OK.value());
        } else {
            log.debug("No resource found for URI: {}", requestUri);
            context.setResponseStatusCode(NOT_FOUND.value());
        }

        return null;
    }
}
