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

package com.intel.podm.common.enterprise.utils.tasks;

import com.intel.podm.common.logger.Logger;

import javax.enterprise.concurrent.ManagedExecutorService;
import javax.enterprise.concurrent.ManagedTaskListener;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.concurrent.CancellationException;
import java.util.concurrent.Future;

@Dependent
public class LoggingTaskListener implements ManagedTaskListener {
    @Inject
    private Logger logger;

    @Override
    public void taskSubmitted(Future<?> future, ManagedExecutorService executor, Object task) {
    }

    @Override
    public void taskAborted(Future<?> future, ManagedExecutorService executor, Object task, Throwable exception) {
        logger.t(task + " was aborted due to {}", exception.toString());

        //According to Java docs, the exception exists when task was cancelled, skipped or aborted (failed to start)
        //We assume that task might be cancelled only by programmer and shouldn't be logged
        //otherwise we should log the cause of termination
        if (!(exception instanceof CancellationException)) {
            logger.e("Task failed to start due to exception.", exception.getCause());
        }
    }

    @Override
    public void taskDone(Future<?> future, ManagedExecutorService executor, Object task, Throwable exception) {
        //Since cancellation is within life cycle of task, we shouldn't log any information about it.
        if (future.isCancelled()) { // :(
            logger.t("Task {} was cancelled", task);
        } else if (exception != null) {
            logger.e("Task {} has failed with exception", task, exception);
        }
    }

    @Override
    public void taskStarting(Future<?> future, ManagedExecutorService executor, Object task) {
    }
}
