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

def get_odata_type(obj):
    """
    Retrieves odata.type from given object. May throw KeyError.
    :param obj: object that potentially contains '@odata.type" property
    :return:
    """
    if not isinstance(obj, dict):
        raise KeyError
    type = obj["@odata.type"]
    return sanitize_odata_type(type)

def is_special_property(property_name):
    return "#" in property_name or "@" in property_name

def sanitize_odata_type(odata_type):
    if odata_type and odata_type.startswith('#'):
        return odata_type[1:]
    return odata_type

def split_path_into_segments(path):
    odata_id_segments = path.split("/")
    # getting rid of empty segments
    return filter(None, odata_id_segments)

def update_existing_keys(dst, src):
    keys_to_update = set(src.keys()).intersection(set(dst.keys()))
    for key in keys_to_update:
        if isinstance(dst[key], dict):
            if isinstance(src[key], dict):
                update_existing_keys(dst[key], src[key])
        else:
            dst[key] = src[key]


def create_data(variable_path, value, collection=None):
    """
    :type variable_path: list [int or str]
    :type value: *
    :type collection: list [*]
    :rtype: dict
    """
    data = value
    # if collection is passed, we want to patch only a single index of this collection but need to pass the whole
    # collection in payload, so we keep the other indexes the same
    if collection:
        collection[variable_path[-1]] = value
        data = collection
        variable_path = variable_path[:-1]

    for path_element in variable_path[::-1]:
        if type(path_element) == int:
            data = ([dict()] * path_element) + [data]
        else:
            data = {path_element: data}

    return data
