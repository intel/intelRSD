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
import os
from glob import glob

from cts_core.metadata.metadata_containers import NamespacesSoupsContainer, EntitiesContainer, TypesContainer, TermsContainer, \
    ActionsContainer, MetadataContainer
from cts_core.metadata.model.action import Action
from cts_core.metadata.model.entity import Entity
from cts_core.metadata.model.metadata_types import ComplexType
from cts_core.metadata.model.metadata_types import EnumType, TypeDefinition
from cts_core.metadata.model.term import Term
from cts_framework.commons.commons import Commons
from cts_framework.tests_helpers.test_case_flags import TestCaseFatalErrorFlag

TYPE_DEFINITION = "typedefinition"
COMPLEX_TYPE = "complextype"
ENUM_TYPE = "enumtype"
SCHEMA = "schema"
NAMESPACE = "namespace"
ENTITY_TYPE = "entitytype"
TERM = "term"


class MetadataManager:
    def __init__(self, configuration):
        config_property_reader = configuration
        self._schema_location_directory = config_property_reader.MetadataLocationDirectory
        self._metadata_files_list = glob(self._schema_location_directory)
        self._soups = []

    def load_files(self):
        # create soup from all files in schema location directory
        soups = [Commons.text_to_soup(Commons.read_file(filename)) for filename in
                 glob(self._schema_location_directory+"/*") if os.path.isfile(filename)]
        # get rid of Nones
        self._soups = [soup for soup in soups if soup]
        if not self._soups:
            raise TestCaseFatalErrorFlag("Unable to read specified metadata - empty resource.")

    def load_namespaces(self):
        namespaces_soup_container = NamespacesSoupsContainer()
        for soup in self._soups:
            for schema in soup.find_all(SCHEMA):
                namespaces_soup_container[schema[NAMESPACE]] = schema

        return namespaces_soup_container

    def load_metadata_properties(self):
        namespaces_soup_container = self.load_namespaces()
        not_analyzed_entities = EntitiesContainer()

        metadata_container = MetadataContainer()

        for namespace_name, namespace_soup in namespaces_soup_container.iteritems():
            not_analyzed_entities .update(self.load_entities(namespace_name, namespace_soup))
            metadata_container.types.update(self.load_types(namespace_name, namespace_soup))
            metadata_container.terms.update(self.load_terms(namespace_name, namespace_soup))
            metadata_container.actions.update(self.load_actions(namespace_name, namespace_soup))

        analyzed_entities = self.analyze_entities(not_analyzed_entities)
        metadata_container.entities = analyzed_entities

        if not metadata_container.entities.values():
            raise TestCaseFatalErrorFlag("Unable to read specified metadata - no entities found.")

        return metadata_container

    def load_terms(self, namespace_name, namespace_soup):
        terms = TermsContainer()
        for term_soup in namespace_soup.find_all(TERM):
            term = Term(namespace_name, term_soup)
            terms[term.name] = term

        return terms

    def analyze_entities(self, not_analyzed_entities):
        analyzed_entities = EntitiesContainer()

        for entity_name, entity in not_analyzed_entities.iteritems():
            if not entity.base_type:
                # adding base entities
                analyzed_entities[entity_name] = entity

        while len(not_analyzed_entities.keys()) != len(analyzed_entities.keys()):
            loop_did_any_affect = False
            for entity_name, entity in not_analyzed_entities.iteritems():
                if entity.base_type in analyzed_entities.keys() and entity_name not in analyzed_entities.keys():
                    # entity ready to be added (base types already analyzed) and entity was not analyzed
                    entity.base_entity_on(analyzed_entities[entity.base_type])
                    analyzed_entities[entity_name] = entity

                    loop_did_any_affect = True

            if not loop_did_any_affect:
                print "WARNING::Metadata contains errors for entities: " + \
                      str(set(not_analyzed_entities.keys()) - set(analyzed_entities.keys())) + \
                      "\n will try anyway to proceed"
                break

        return analyzed_entities

    def load_entities(self, namespace_name, namespace_soup):
        entities = EntitiesContainer()
        for entity_soup in namespace_soup.find_all(ENTITY_TYPE):
            entity = Entity(namespace_name, entity_soup)
            entities[entity.name] = entity

        return entities

    def load_types(self, namespace_name, namespace_soup):
        types = TypesContainer()
        for type_soup in namespace_soup.find_all(ENUM_TYPE):
            enum_type = EnumType(namespace_name, type_soup)
            types[enum_type.name] = enum_type

        for type_soup in namespace_soup.find_all(COMPLEX_TYPE):
            complex_type = ComplexType(namespace_name, type_soup)
            types[complex_type.name] = complex_type

        for type_soup in namespace_soup.find_all(TYPE_DEFINITION):
            type_definition = TypeDefinition(namespace_name, type_soup)
            types[type_definition.name] = type_definition

        return types

    def load_actions(self, namespace_name, namespace_soup):
        actions = ActionsContainer()
        for action_soup in namespace_soup.find_all("action"):
            action = Action(namespace_name, action_soup)
            actions[action.name] = action

        return actions
