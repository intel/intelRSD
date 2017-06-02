/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.common.logger;


/**
 * Implementation of {@link Logger} backed by {@link org.slf4j.Logger}.
 */
public class LoggerImpl implements Logger {

    protected final org.slf4j.Logger decorated;

    public LoggerImpl(org.slf4j.Logger log) {
        this.decorated = log;
    }

    @Override
    public void t(String msg) {
        decorated.trace(msg);
    }

    @Override
    public void t(String format, Object... arguments) {
        decorated.trace(format, arguments);
    }

    @Override
    public void d(String msg) {
        decorated.debug(msg);
    }

    @Override
    public void d(String format, Object... arguments) {
        decorated.debug(format, arguments);
    }

    @Override
    public void i(String msg) {
        decorated.info(msg);
    }

    @Override
    public void i(String format, Object... arguments) {
        decorated.info(format, arguments);
    }

    @Override
    public void w(String msg) {
        decorated.warn(msg);
    }

    @Override
    public void w(String format, Object... arguments) {
        decorated.warn(format, arguments);
    }

    @Override
    public void e(String msg) {
        decorated.error(msg);
    }

    @Override
    public void e(String format, Object... arguments) {
        decorated.error(format, arguments);
    }

    @Override
    public void e(String msg, Throwable t) {
        decorated.error(msg, t);
    }
}
