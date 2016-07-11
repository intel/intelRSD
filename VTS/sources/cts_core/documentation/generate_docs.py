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

from jinja2 import Template

from cts_core.documentation.annotations_template import ANNOTATIONS_TEMPLATE
from cts_core.documentation.entities_template import ENTITIES_TEMPLATE
from cts_core.documentation.main_template import MAIN_TEMPLATE
from cts_core.documentation.menu_template import MENU_TEMPLATE
from cts_core.documentation.navigation_property_template import NAVIGATION_PROPERTY_TEMPLATE
from cts_core.documentation.property_template import PROPERTY_TEMPLATE
from cts_core.documentation.types_template import TYPES_TEMPLATE



class GenerateDocs:
    def __init__(self):
        self.kwargs = dict(render_annotations=self.render_annotations,
                           render_types=self.render_types,
                           render_entities=self.render_entities,
                           render_property=self.render_property,
                           render_navigation_property=self.render_navigation_property,
                           render_menu=self.render_menu)

    def generate_documentation(self, metadata_container):
        return self.render(MAIN_TEMPLATE, dict(types=metadata_container.types.values(),
                                               entities=metadata_container.entities.values()))


    def render_annotations(self, annotations):
        return self.render(ANNOTATIONS_TEMPLATE, dict(annotations=annotations))

    def render_types(self, types):
        return self.render(TYPES_TEMPLATE, dict(types=types))

    def render_entities(self, entities):
        return self.render(ENTITIES_TEMPLATE, dict(entities=entities))

    def render_property(self, property):
        return self.render(PROPERTY_TEMPLATE, dict(property=property))

    def render_navigation_property(self, property):
        return self.render(NAVIGATION_PROPERTY_TEMPLATE, dict(property=property))

    def render_menu(self, types, entities):
        entities = sorted([entity.name for entity in entities])
        return self.render(MENU_TEMPLATE, dict(types=types, entities=entities))

    def render(self, template, extra_kwargs):
        kwargs = self.kwargs
        kwargs.update(extra_kwargs)
        return Template(template).render(**kwargs)
