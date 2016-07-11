"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

PROPERTY_TEMPLATE = """
<div>
    <h3>{{property.name}}</h3>
    <table>
        <tr><td>is collection?</td><td>{{property.is_collection}}</td></tr>
        <tr><td>type:</td><td><a href="#type_{{property.type}}">{{property.type}}</a></td></tr>
        <tr><td>nullable:</td><td>{{property.nullable}}</td></tr>
        <tr><td>required:</td><td>{{property.required}}</td></tr>
    </table>
    {{ render_annotations(property.annotations) }}
</div>
"""
