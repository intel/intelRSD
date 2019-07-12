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

import lombok.Getter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static java.util.Collections.unmodifiableList;

public enum QueryParameterType {
    SKIP("$skip"),
    TOP("$top"),
    ONLY("only"),
    EXPAND("$expand"),
    SELECT("$select"),
    FILTER("$filter"),
    EXCERPT("excerpt");

    private static final List<QueryParameterType> SUPPORTED_QUERY_PARAMS;
    @Getter
    private final String value;

    static {
        List<QueryParameterType> list = new ArrayList<>();
        list.add(FILTER);

        SUPPORTED_QUERY_PARAMS = unmodifiableList(list);
    }

    QueryParameterType(String value) {
        this.value = value;
    }

    public String toString() {
        return getValue();
    }

    public static List<QueryParameterType> getSupportedQueryParams() {
        return SUPPORTED_QUERY_PARAMS;
    }

    public static QueryParameterType getParameterTypeByString(String type) {
        return Arrays.stream(QueryParameterType.values())
            .filter(queryParam -> queryParam.getValue().equals(type))
            .findFirst()
            .orElse(null);
    }
}
