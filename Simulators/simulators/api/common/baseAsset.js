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

const uuid = require('uuid');

class BaseAsset {
    constructor(mock, context, id, properties, specifiedUuid) {
        this.mock = mock;
        this.context = context;
        this.id = id;
        this.properties = properties;
        this.jsonTemplate = {};
        if (specifiedUuid == null) {
            this.uuid = uuid.v1();
        } else {
            this.uuid = specifiedUuid;
        }
    }

    get Id() {
        return this.id;
    }

    get Uuid() {
        return this.uuid;
    }

    get JsonTemplate() {
        return this.jsonTemplate;
    }

    set JsonTemplate(jsonTemplate) {
        this.jsonTemplate = jsonTemplate;
    }

    get Content () {
        return this.CreateJsonTemplate.bind(this);
    }

    get Parent() {
        return this.context;
    }

    CreateJsonTemplate() {
        return this.JsonTemplate;
    }

    MockGetWithDynamicBody() {
        return this.mock.mockGetWithDynamicBody(this.GetBaseUri(), this.Content);
    }

    GetBaseUri(collectionName) {
        return `${this.context.GetBaseUri()}/${collectionName}/${this.id}`;
    }
}

exports.BaseAsset = BaseAsset;
