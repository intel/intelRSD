/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.Dirtying;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Scope;
import org.springframework.core.task.TaskExecutor;
import org.springframework.stereotype.Component;

import java.util.function.Consumer;

import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.ASYNC_TASKS_EXECUTOR;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class NodeAssemblyTaskRunner implements Consumer<ChainElement<NodeTask>> {
    private final TaskExecutor asyncTaskExecutor;

    @Autowired
    public NodeAssemblyTaskRunner(@Qualifier(ASYNC_TASKS_EXECUTOR) TaskExecutor asyncTaskExecutor) {
        this.asyncTaskExecutor = asyncTaskExecutor;
    }

    @Override
    @Dirtying
    public void accept(ChainElement<NodeTask> chainElement) {
        asyncTaskExecutor.execute(chainElement);
    }
}
