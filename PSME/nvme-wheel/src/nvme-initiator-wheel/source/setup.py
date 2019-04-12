"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from setuptools import setup

setup(
    name='NVME Wheel',
    description='NVME Wheel package written in Python',
    version='1.3.21',
    author='Tomasz (tmacieje) Maciejewski',
    author_email='tomasz.maciejewski@intel.com',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Programming Language :: Python :: 2.7'
    ],
    packages=['nvme_main',
              'nvme_framework',
              'nvme_framework.actions',
              'nvme_framework.actions.execute',
              'nvme_framework.configuration',
              'nvme_framework.helpers',
              'nvme_framework.rest'],
    url='',
    install_requires=['Flask'],
    license='',
    entry_points={
        'console_scripts': [
            'nvme-wheel = nvme_main.main:main',
        ],
    },
)
