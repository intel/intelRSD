/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.services.detection;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.detection.tasks.CheckIfServiceStillExistTask;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.concurrent.RejectedExecutionException;

@ApplicationScoped
public class ServiceEndpointChecker {

    @Inject
    private Logger logger;

    @Inject
    private BeanFactory beanFactory;

    @Resource
    private ManagedScheduledExecutorService executorService;

    public void triggerCheckIfServiceStillExist(ServiceEndpoint endpoint) {
        CheckIfServiceStillExistTask task = beanFactory.create(CheckIfServiceStillExistTask.class);
        task.setEndpointToCheck(endpoint);
        logger.t("Executing one shot task to check URIs");
        try {
            executorService.execute(task);
        } catch (RejectedExecutionException e) {
            logger.e("Managed executor refuses to execute task.", e);
            throw e;
        }
    }
}
