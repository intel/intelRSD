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

package liquibase.ext.logging;

import liquibase.logging.core.AbstractLogger;
import org.slf4j.Logger;

import static java.lang.Integer.MAX_VALUE;
import static org.slf4j.LoggerFactory.getLogger;

/**
 * This logger is automatically loaded by Liquibase instance.
 * Do not remove this class!!!
 */
public class LiquibaseLogger extends AbstractLogger {

    private Logger logger;

    @Override
    public void setName(String name) {
        this.logger = getLogger(name);
    }

    @Override
    public void setLogLevel(String logLevel, String logFile) {
        //Do nothing
    }

    @Override
    public void severe(String message) {
        logger.error(buildMessage(message));
    }

    @Override
    public void severe(String message, Throwable e) {
        logger.error(buildMessage(message), e);
    }

    @Override
    public void warning(String message) {
        logger.warn(buildMessage(message));
    }

    @Override
    public void warning(String message, Throwable e) {
        logger.warn(buildMessage(message), e);
    }

    @Override
    public void info(String message) {
        logger.info(buildMessage(message));
    }

    @Override
    public void info(String message, Throwable e) {
        logger.info(buildMessage(message), e);
    }

    @Override
    public void debug(String message) {
        logger.debug(buildMessage(message));
    }

    @Override
    public void debug(String message, Throwable e) {
        logger.debug(buildMessage(message), e);
    }

    @Override
    public int getPriority() {
        return MAX_VALUE;
    }
}
