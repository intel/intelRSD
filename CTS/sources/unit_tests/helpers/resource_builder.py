"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

from cts_core.discovery.api_resource import ApiResource
from copy import copy, deepcopy

class ResourceBuilder:
    def __init__(self, container):
        self.container = container

    def build(self, odata_id, odata_type, odata_context, **kwargs):
        self.odata_id = odata_id
        self.odata_type = odata_type
        self.odata_context = odata_context

        self.resource = kwargs #{k : v for (k,v) in kwargs.iteritems()}
        self.resource.update({'@odata.id' : odata_id, '@odata.type' : odata_type, '@odata.context' : odata_context})
        return self

    def with_location(self, id, parent_id):
        self.resource.update({'Oem' : {'Intel_RackScale' : {'Location' : {'Id' : id, 'ParentId' : parent_id}}}})
        return self

    def contains(self, *args):
        self.resource.setdefault('Links', {}).update({'Contains' : [{'@odata.id' : id} for id in args]})
        return self

    def contained_by(self, parent):
        self.resource.setdefault('Links', {}).update({'ContainedBy' : {'@odata.id' : parent}})
        return self

    def commit(self):
        self.container.add_resource(ApiResource(self.odata_id, 'netloc', self.resource,
                                                self.odata_type))
        return self

    def clone(self, odata_id, **kwargs):
        cloned = ResourceBuilder(self.container)
        cloned.build(copy(odata_id), copy(self.odata_type), copy(self.odata_context), **deepcopy(self.resource))
        cloned.resource.update(kwargs)
        return cloned


