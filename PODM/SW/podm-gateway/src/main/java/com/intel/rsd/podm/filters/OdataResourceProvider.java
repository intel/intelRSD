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

import lombok.SneakyThrows;
import org.springframework.boot.web.servlet.FilterRegistrationBean;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

import static java.util.Collections.singletonList;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;

public class OdataResourceProvider extends FilterRegistrationBean<OdataResourceProvider> implements Filter {

    public static final String REDFISH_ODATA_RESOURCE_URI = "/redfish/v1/odata";

    private final String odataJsonAsString;

    @SneakyThrows
    public OdataResourceProvider(String odataJsonAsString) {
        setFilter(this);
        setUrlPatterns(singletonList(REDFISH_ODATA_RESOURCE_URI));
        this.odataJsonAsString = odataJsonAsString;
    }

    @Override
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) throws IOException {
        if (request instanceof HttpServletRequest) {
            ((HttpServletResponse) response).setStatus(OK.value());
            response.setContentType(APPLICATION_JSON_VALUE);
            response.getWriter().write(odataJsonAsString);
            response.getWriter().flush();
            response.getWriter().close();
            return;
        }
    }

    @Override
    public void destroy() {
        //do nothing
    }

    @Override
    public void init(FilterConfig filterConfig) {
        //do nothing
    }
}
