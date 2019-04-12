/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.services.common.hazelcast;

import com.google.common.collect.Iterables;
import com.hazelcast.scheduledexecutor.IScheduledExecutorService;
import com.hazelcast.scheduledexecutor.IScheduledFuture;
import lombok.val;
import org.springframework.context.ApplicationContext;
import org.springframework.scheduling.TaskScheduler;

import java.time.Duration;
import java.util.Objects;
import java.util.Set;

import static com.google.common.collect.Sets.difference;
import static com.google.common.collect.Streams.stream;
import static com.hazelcast.scheduledexecutor.TaskUtils.named;
import static java.lang.String.format;
import static java.util.concurrent.TimeUnit.MILLISECONDS;
import static java.util.stream.Collectors.toSet;

public abstract class DistributedTaskSchedulerTask extends ScheduledWithFixedDelayOnLeaderTask {
    private final IScheduledExecutorService executorService;
    private ApplicationContext applicationContext;

    public DistributedTaskSchedulerTask(TaskScheduler taskScheduler, IScheduledExecutorService executorService, ApplicationContext applicationContext) {
        super(taskScheduler);
        this.executorService = executorService;
        this.applicationContext = applicationContext;
    }

    @Override
    public final void run() {
        val tasksToCancel = difference(getIdsOfCurrentlyRunningTasks(), getIdsOfTasksThatShouldBeRunning());
        val tasksToStart = difference(getIdsOfTasksThatShouldBeRunning(), getIdsOfCurrentlyRunningTasks());

        tasksToCancel.forEach(this::cancel);
        tasksToStart.forEach(this::schedule);
    }

    @Override
    public String toString() {
        return format("%s Scheduler", getTypeNameOfDistributedTasks());
    }

    protected String getPrettyTaskName(String taskId) {
        return format("%s(%s)", getTypeNameOfDistributedTasks(), taskId);
    }

    protected abstract String getTypeNameOfDistributedTasks();

    protected abstract Set<String> getIdsOfTasksThatShouldBeRunning();

    protected abstract Runnable createTask(String taskId);

    protected abstract Duration getDistributedTaskDelay();

    private void schedule(String taskId) {
        val task = createTask(taskId);
        autowireBean(task);
        executorService.scheduleOnKeyOwnerAtFixedRate(named(taskId, task), taskId, 0, getDistributedTaskDelay().toMillis(), MILLISECONDS);
        log.info("Task {} has been scheduled", getPrettyTaskName(taskId));
    }

    private void autowireBean(Object object) {
        applicationContext.getAutowireCapableBeanFactory().autowireBean(object);
    }

    private void cancel(String taskId) {
        val task = findTask(taskId);
        if (task != null) {
            cancel(task);
        } else {
            log.error("Could not find task {}", taskId);
        }
    }

    private void cancel(IScheduledFuture<?> task) {
        if (task.cancel(false)) {
            log.info("Task {} has been cancelled", getPrettyTaskName(nameOf(task)));
            task.dispose(); //TODO: hkrasnod - is it safe to run it while task is running?
        } else if (!task.isCancelled()) {
            log.error("{} has not been canceled", nameOf(task));
        }
    }

    private IScheduledFuture<?> findTask(String taskId) {
        val runningTasks = getCurrentlyRunningTasks();
        return Iterables.find(runningTasks, t -> Objects.equals(nameOf(t), taskId), null);
    }

    private Iterable<IScheduledFuture<?>> getCurrentlyRunningTasks() {
        return Iterables.concat(executorService.getAllScheduledFutures().values());
    }

    private Set<String> getIdsOfCurrentlyRunningTasks() {
        return stream(getCurrentlyRunningTasks())
            .map(this::nameOf)
            .collect(toSet());
    }

    private String nameOf(IScheduledFuture<?> task) {
        return task.getHandler().getTaskName();
    }
}
