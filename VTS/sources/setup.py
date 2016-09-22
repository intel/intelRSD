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

from setuptools import setup

setup(
    name='Vts',
    version='1.0.0',
    packages=['main', 'cts_core', 'cts_framework.tests_helpers', 'cts_core.commons', 'cts_core.metadata',
              'cts_core.metadata.model', 'cts_core.metadata.primitive_types_helpers', 'cts_core.discovery',
              'cts_framework', 'cts_framework.db', 'cts_framework.db.dao', 'cts_framework.db.model',
              'cts_framework.actions', 'cts_framework.actions.execute', 'cts_framework.actions.status',
              'cts_framework.actions.tests', 'cts_framework.commons', 'cts_framework.logging',
              'cts_framework.configuration', 'cts_framework.tests_helpers', 'cts_framework.tests_running',
              'cts_framework.tests_managing', 'cts_core.documentation', 'cts_core.metadata_validation',
              'cts_core.metadata_validation.get', 'cts_core.metadata_validation.get.validators',
              'cts_core.metadata_validation.patch'],
    url='',
    install_requires=["lxml", "bs4", "configparser", "requests", "jinja2", "tabulate", "sqlalchemy", "simplejson",
                      "rstr"],
    license='',
    description='Validation Test Suite',
    entry_points={
        'console_scripts': [
            'vts = main.main:main',
        ],
    },
)
