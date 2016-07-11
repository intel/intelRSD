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

ENTITIES_TEMPLATE = """
<div id="entities">
    {% for entity in entities %}
        <div id="#entity_{{entity.name}}">
            <h2>{{entity.name}}</h2>
                {% if entity.properties %}
                    <h3>Properties:</h3>
                    <ul>
                        {% for property in entity.properties %}
                        {{render_property(property)}}
                        {% endfor %}
                    </ul>
                {% endif %}

                {% if entity.navigation_properties %}
                    <h3>Navigation properties:</h3>
                    <ul>
                        {% for property in entity.navigation_properties %}
                        {{render_navigation_property(property)}}
                        {% endfor %}
                    </ul>
                {% endif %}

        </div>
    {% endfor %}
</div>
"""
