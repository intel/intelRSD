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
from cts_core.metadata.model.metadata_types.primitive_types.primitive_type import PrimitiveType


class Commons:
    """
    common class for metadata model
    """
    @staticmethod
    def parse_type_name(type_name, namespace_name):
        if not namespace_name:
            return type_name

        if "." not in type_name and type_name not in PrimitiveType.PRIMITIVE_TYPE_NAME_LIST:
            type_name = ".".join([namespace_name, type_name])

        return type_name
