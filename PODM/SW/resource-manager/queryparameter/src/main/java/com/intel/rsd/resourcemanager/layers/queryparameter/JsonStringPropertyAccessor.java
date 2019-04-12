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

import com.fasterxml.jackson.databind.JsonNode;
import lombok.val;
import org.springframework.expression.AccessException;
import org.springframework.expression.EvaluationContext;
import org.springframework.expression.TypedValue;
import org.springframework.integration.json.JsonPropertyAccessor;
import org.springframework.stereotype.Component;

import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.RANDOM_STRING;
import static java.lang.Boolean.parseBoolean;
import static java.lang.Double.parseDouble;
import static org.springframework.expression.TypedValue.NULL;

@Component
public class JsonStringPropertyAccessor extends JsonPropertyAccessor {
    private final RedfishSpelPropertyConverter converter;

    public JsonStringPropertyAccessor(RedfishSpelPropertyConverter converter) {
        this.converter = converter;
    }

    @Override
    public boolean canRead(EvaluationContext context, Object target, String name) throws AccessException {
        return super.canRead(context, target, converter.convertRedfishAllowableValuesFrom(name, RANDOM_STRING));
    }

    @Override
    public TypedValue read(EvaluationContext context, Object target, String name) throws AccessException {
        val result = super.read(context, target, converter.convertRedfishAllowableValuesFrom(name, RANDOM_STRING));
        val resultValue = result.getValue();
        if (resultValue instanceof JsonPropertyAccessor.ToStringFriendlyJsonNode) {
            val node = ((ToStringFriendlyJsonNode) resultValue).getTarget();
            return defineResultType(node, resultValue.toString());
        }

        return result;
    }

    private TypedValue defineResultType(JsonNode node, String stringValue) {
        switch (node.getNodeType()) {
            case NUMBER: return new TypedValue(parseDouble(stringValue));
            case BOOLEAN: return new TypedValue(parseBoolean(stringValue));
            default: return stringOrNullTypedValue(stringValue);
        }
    }

    private TypedValue stringOrNullTypedValue(String value) {
        if ("null".equals(value)) {
            return NULL;
        }
        return new TypedValue(value);
    }
}
