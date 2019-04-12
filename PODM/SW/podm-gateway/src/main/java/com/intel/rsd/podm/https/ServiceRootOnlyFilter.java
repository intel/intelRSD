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

package com.intel.rsd.podm.https;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.stereotype.Component;
import org.springframework.web.util.UriComponentsBuilder;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

@Component
@ConditionalOnProperty(prefix = "server.ssl.", name = {"enabled", "key-store", "key-password", "key-store-password"})
public class ServiceRootOnlyFilter implements Filter {

    @Autowired
    private SslConfiguration sslConfiguration;

    @Override
    public void init(FilterConfig filterConfig) throws ServletException {
    }

    @Override
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) throws IOException, ServletException {
        if (request instanceof HttpServletRequest) {
            HttpServletRequest httpServletRequest = (HttpServletRequest) request;
            if (shouldRedirect(httpServletRequest)) {
                String newUrl = UriComponentsBuilder.fromUriString(httpServletRequest.getRequestURL().toString())
                    .scheme("https").port(sslConfiguration.getHttpsPort()).build().toUriString();
                ((HttpServletResponse) response).sendRedirect(newUrl);
                return;
            }
        }
        chain.doFilter(request, response);
    }

    public boolean shouldRedirect(HttpServletRequest httpServletRequest) {
        return !httpServletRequest.getRequestURI().equals("/redfish/v1") && httpServletRequest.getScheme().equals("http");
    }

    @Override
    public void destroy() {

    }
}
