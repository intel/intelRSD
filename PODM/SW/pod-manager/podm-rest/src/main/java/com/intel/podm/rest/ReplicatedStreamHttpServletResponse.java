/*
 * Copyright (c) 2016 Intel Corporation
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

import org.apache.commons.io.output.TeeOutputStream;

import javax.servlet.ServletOutputStream;
import javax.servlet.WriteListener;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletResponseWrapper;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;

public class ReplicatedStreamHttpServletResponse extends HttpServletResponseWrapper implements AutoCloseable {
    private final ByteArrayOutputStream byteArrayOutputStream;
    private final PrintStream printStream;

    ReplicatedStreamHttpServletResponse(HttpServletResponse response) {
        super(response);
        byteArrayOutputStream = new ByteArrayOutputStream();
        printStream = new PrintStream(byteArrayOutputStream);
    }

    @Override
    public ServletOutputStream getOutputStream() throws IOException {
        return new ReplicatedServletOutputStream(new TeeOutputStream(super.getOutputStream(), printStream));
    }

    @Override
    public PrintWriter getWriter() throws IOException {
        return new PrintWriter(new ReplicatedServletOutputStream(new TeeOutputStream(super.getOutputStream(), printStream)));
    }

    @Override
    public String toString() {
        return byteArrayOutputStream.toString();
    }

    @Override
    public void close() throws Exception {
        printStream.close();
    }

    private class ReplicatedServletOutputStream extends ServletOutputStream {
        private final OutputStream targetStream;

        ReplicatedServletOutputStream(OutputStream targetStream) {
            this.targetStream = targetStream;
        }

        @Override
        public void write(byte[] b) throws IOException {
            targetStream.write(b);
        }

        @Override
        public void write(byte[] b, int off, int len) throws IOException {
            targetStream.write(b, off, len);
        }

        @Override
        public void flush() throws IOException {
            targetStream.flush();
        }

        @Override
        public void close() throws IOException {
            targetStream.close();
        }

        @Override
        public void write(int b) throws IOException {
            targetStream.write(b);
        }

        @Override
        public boolean isReady() {
            return false;
        }

        @Override
        public void setWriteListener(WriteListener writeListener) {
        }
    }
}
