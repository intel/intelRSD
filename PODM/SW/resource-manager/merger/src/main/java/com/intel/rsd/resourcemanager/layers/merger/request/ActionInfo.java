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

package com.intel.rsd.resourcemanager.layers.merger.request;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.NonNull;

import java.util.regex.Matcher;

import static com.intel.rsd.ResourcePatterns.ACTION_PATTERN;

public class ActionInfo {

    private final String actionName;
    private final String resourcePath;
    private final String actionPath;

    public ActionInfo(String actionPath) {
        Matcher matcher = ACTION_PATTERN.matcher(actionPath);
        if (!matcher.matches()) {
            throw new IllegalArgumentException("Given path is not an action path: " + actionPath);
        }
        this.actionPath = actionPath;
        this.actionName = matcher.group("actionName");
        this.resourcePath = matcher.group("resourcePath");
    }

    public String getActionName() {
        return actionName;
    }

    public String getResourcePath() {
        return resourcePath;
    }

    public static boolean isActionUri(String path) {
        return ACTION_PATTERN.matcher(path).matches();
    }

    public boolean isSupportedBy(@NonNull JsonNode body) {
        return body.findValue("#" + actionName) != null;
    }

    @Override
    public String toString() {
        return actionPath;
    }
}
