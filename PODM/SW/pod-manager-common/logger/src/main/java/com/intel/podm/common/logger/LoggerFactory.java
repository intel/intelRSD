/*
 * Copyright (c) 2015-2018 Intel Corporation
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
 * Wrapper of {@link org.slf4j.LoggerFactory}.
 *
 * Returns simplified {@link Logger} backed by {@link org.slf4j.Logger}.
 */
public final class LoggerFactory {

    private LoggerFactory() {
        throw new UnsupportedOperationException();
    }

    public static Logger getLogger(final Class<?> clazz) {
        return new LoggerImpl(org.slf4j.LoggerFactory.getLogger(clazz));
    }

    public static ServiceLifecycleLogger getServiceLifecycleLogger(final Class<?> clazz) {
        return new ServiceLifecycleLoggerImpl(org.slf4j.LoggerFactory.getLogger(clazz));
    }

    public static Logger getLogger(final String loggerName) {
        return new LoggerImpl(org.slf4j.LoggerFactory.getLogger(loggerName));
    }
}
