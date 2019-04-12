/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer;

import lombok.extern.slf4j.Slf4j;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.io.Serializable;

@Aspect
@Component
@Slf4j
@Order(50)
public class DirtyingAspect implements Serializable {

    private static final long serialVersionUID = 2495996382984369338L;
    private final ModelState modelState;

    @Autowired
    public DirtyingAspect(ModelState modelState) {
        this.modelState = modelState;
    }

    @Before("execution(* *(..)) && @annotation(dirty)")
    public void invoke(JoinPoint joinPoint, Dirtying dirty) {
        modelState.dirty(joinPoint.toShortString());
    }
}
