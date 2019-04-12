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

package com.intel.rsd.nodecomposer.types.actions;

public final class ComposedNodeActionNames {
    public static final String COMPOSED_NODE_RESET = "ComposedNode.Reset";
    public static final String COMPOSED_NODE_ASSEMBLE = "ComposedNode.Assemble";
    public static final String COMPOSED_NODE_ATTACH_RESOURCE = "ComposedNode.AttachResource";
    public static final String COMPOSED_NODE_DETACH_RESOURCE = "ComposedNode.DetachResource";
    public static final String ATTACH_RESOURCE_ACTION_INFO = "AttachResourceActionInfo";
    public static final String DETACH_RESOURCE_ACTION_INFO = "DetachResourceActionInfo";
    public static final String COMPOSED_NODE_FORCE_DELETE = "ComposedNode.ForceDelete";

    private ComposedNodeActionNames() {
    }
}
