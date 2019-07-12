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
from functools import partial

from cts_core.commons.error import cts_error, cts_warning
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.model.metadata_types import EnumType, ComplexType, TypeDefinition
from cts_core.metadata.model.metadata_types.primitive_types.primitive_type import PrimitiveType
# force all primitive types to be loaded and registered
from cts_core.metadata.model.metadata_types.primitive_types import *
from cts_framework.commons.progress_bar import ProgressBar


class EntitiesContainer(dict):
    def __init__(self, type_mapping_func, *args, **kwargs):
        dict.__init__(self, *args, **kwargs)
        self.type_mapping_func = type_mapping_func

    def __getitem__(self, item):
        """
        :rtype: cts_core.metadata.model.entity.Entity
        """
        mapped_type = self.type_mapping_func(item)
        try:
            return dict.__getitem__(self, mapped_type)
        except KeyError as key:
            if item is not None:
                cts_error("Unknown entity type {key}", **locals())
            raise KeyError(key)

    def compare(self, other, progress_bar):
        '''
        :type other: cts_core.metadata.metadata_container.EntitiesContainer
        :return: int
        '''
        cmp = Comparator(level=0)

        entities = set(self.keys())
        entities_other = set(other.keys())
        all = sorted(list(entities.union(entities_other)))
        both = entities.intersection(entities_other)

        fmt = "Entity: {entity}"
        for entity in all:
            if progress_bar is not None:
                progress_bar.set_label(entity)

            if entity in both:
                cmp.merge_result(self.__getitem__(entity).compare(other.__getitem__(entity)))
            elif entity in entities:
                cmp.set_not_equal()
                cmp.message_left(fmt.format(entity=entity))
                cmp.message_right('?')
                cmp.separator()
            else:
                cmp.set_not_equal()
                cmp.message_left('?')
                cmp.message_right(fmt.format(entity=entity))
                cmp.separator()

            if progress_bar is not None:
                progress_bar.make_progress()
        return cmp


class TypesContainer(dict):
    def __init__(self, type_mapping_func, *params, **kwargs):
        dict.__init__(self, *params, **kwargs)

        for primitive_type in PrimitiveType.PRIMITIVE_TYPES_LIST:
            primitive_type_instance = primitive_type()
            self[primitive_type_instance.name] = primitive_type_instance

        self.type_mapping_func = type_mapping_func

    def __getitem__(self, item):
        """
        :rtype: EnumType or ComplexType or TypeDefinition or PrimitiveType
        """
        mapped_type = self.type_mapping_func(item)
        try:
            return dict.__getitem__(self, mapped_type)
        except KeyError as key:
            if item is not None:
                cts_error("Unknown type {key}", **locals())
            raise KeyError(key)

    def compare(self, other, progress_bar):
        '''
        :type other: cts_core.metadata.metadata_container.TypesContainer
        :return: cts_core.metadata.compare_result.CompareResult
        '''

        cmp = Comparator(level=0)

        types = set(self.keys())
        types_other = set(other.keys())
        all = sorted(list(types.union(types_other)))
        both = types.intersection(types_other)

        fmt = "Type: {type}"
        for type in all:
            if progress_bar is not None:
                progress_bar.set_label(type)

            if type in both:
                cmp.merge_result(self.__getitem__(type).compare(other.__getitem__(type)))
            elif type in types:
                cmp.set_not_equal()
                cmp.message_left(fmt.format(type=type))
                cmp.message_right('?')
                cmp.separator()
            else:
                cmp.set_not_equal()
                cmp.message_left('?')
                cmp.message_right(fmt.format(type=type))
                cmp.separator()

            if progress_bar is not None:
                progress_bar.make_progress()
        return cmp


class MetadataContainer:
    def __init__(self, ignore_types=None, map_types=None):
        self._ignore_types = ignore_types if ignore_types is not None else set()
        self._map_types = map_types if map_types is not None else dict()

        mapping_func = partial(MetadataContainer.map_type, self)
        self.entities = EntitiesContainer(type_mapping_func=mapping_func)
        self.types = TypesContainer(type_mapping_func=mapping_func)

    def _print_entities(self):
        print "DEBUG::Known Entity Types:"
        for key in sorted(self.entities.iterkeys()):
            print "DEBUG::\t%s" % key

    def print_types(self):
        self._print_entities()
        print "DEBUG::Other types:"
        for key in sorted(self.types.iterkeys()):
            print "DEBUG::\t%s" % key

    @property
    def get_ignored_types(self):
        return self._ignore_types

    def to_be_ignored(self, *types):
        ignored_types = self.get_ignored_types
        for type in set(types):
            if not type:
                continue
            if self._wide_types(type, ignored_types):
                return True
            elif type in ignored_types:
                cts_warning("User declared to skip validation of type {type}", type=type)
                return True
        return False

    @staticmethod
    def _wide_types(type, ignored_types):
        import re
        for i in ignored_types:
            prog = re.compile(i)
            if prog.match(type):
                cts_warning("User declared to skip validation of type {type} using this pattern {pattern}",
                            type=type,
                            pattern=prog.pattern)
                return True
        return False

    def map_type(self, type):
        if type in self._map_types:
            mapped_type = self._map_types[type]
            cts_warning("User declared to use {mapped} to validate original",
                        mapped=mapped_type, original=type)
            return mapped_type
        return type

    def get_type_definition(self, type_name):
        if type_name in self.types:
            return self.types[type_name]
        if type_name in self.entities:
            return self.entities[type_name]
        raise KeyError(type_name)

    def compare(self, other, with_progress_bar=False):
        '''
        :param other: cts_core.metadata.metadata_container.MetadataContainer
        :param with_progress_bar:
        :return: cts_core.metadata.compare_result.CompareResult
        '''
        if with_progress_bar:
            entities = set(self.entities.keys())
            entities_other = set(other.entities.keys())
            num_entities = len(list(entities.union(entities_other)))

            types = set(self.types.keys())
            types_other = set(other.types.keys())
            num_types = len(list(types.union(types_other)))

            progress_bar = ProgressBar(50, num_steps=num_entities + num_types)
            progress_bar.draw()
        else:
            progress_bar = None

        cmp = Comparator()

        entities_compare_results = self.entities.compare(other.entities, progress_bar)
        types_compare_results = self.types.compare(other.types, progress_bar)

        cmp.merge_result(entities_compare_results, types_compare_results)
        return cmp
