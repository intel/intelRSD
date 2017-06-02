/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.rest;

import javax.servlet.ReadListener;
import javax.servlet.ServletInputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletRequestWrapper;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import static org.apache.commons.io.IOUtils.toByteArray;

public class ResettableStreamHttpServletRequest extends HttpServletRequestWrapper {
    private byte[] rawData;
    private HttpServletRequest httpServletRequest;
    private ResettableServletInputStream servletStream;

    public ResettableStreamHttpServletRequest(HttpServletRequest request) {
        super(request);
        httpServletRequest = request;
        servletStream = new ResettableServletInputStream();
    }

    @Override
    public ServletInputStream getInputStream() throws IOException {
        return getStream();
    }

    @Override
    public BufferedReader getReader() throws IOException {
        return new BufferedReader(new InputStreamReader(getStream()));
    }

    public void resetInputStream() {
        servletStream.setStream(new ByteArrayInputStream(rawData));
    }

    private ServletInputStream getStream() throws IOException {
        if (rawData == null) {
            rawData = toByteArray(httpServletRequest.getReader());
            servletStream.setStream(new ByteArrayInputStream(rawData));
        }
        return servletStream;
    }

    private class ResettableServletInputStream extends ServletInputStream {
        private InputStream stream;

        @Override
        public int read() throws IOException {
            return stream.read();
        }

        @Override
        public boolean isFinished() {
            return false;
        }

        @Override
        public boolean isReady() {
            return false;
        }

        @Override
        public void setReadListener(ReadListener readListener) {
        }

        private void setStream(InputStream inputStream) {
            stream = inputStream;
        }
    }
}
