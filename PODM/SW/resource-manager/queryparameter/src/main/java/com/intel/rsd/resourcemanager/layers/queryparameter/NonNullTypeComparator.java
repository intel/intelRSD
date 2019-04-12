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

package com.intel.rsd.resourcemanager.layers.queryparameter;

import org.springframework.expression.EvaluationException;
import org.springframework.expression.spel.SpelEvaluationException;
import org.springframework.expression.spel.support.StandardTypeComparator;
import org.springframework.stereotype.Component;

import static org.springframework.expression.spel.SpelMessage.NOT_COMPARABLE;

@Component
public class NonNullTypeComparator extends StandardTypeComparator {
    @Override
    public boolean canCompare(Object firstObject, Object secondObject) {
        return super.canCompare(firstObject, secondObject);
    }

    @Override
    public int compare(Object firstObject, Object secondObject) throws EvaluationException {
        if (firstObject == null || secondObject == null) {
            throw new SpelEvaluationException(NOT_COMPARABLE);
        }
        return super.compare(firstObject, secondObject);
    }
}
