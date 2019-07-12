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
import collections
import json
from datetime import datetime
from os import makedirs
from os.path import exists
from shutil import copytree

import pkg_resources
from jinja2 import Template

from cts_core.commons.error import ErrorMute
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.diff.metadata_comparator import MetadataComparator
from cts_framework.actions.action import Action
from cts_framework.actions.status.results_formatters.text_formatter import PARAM_USER, PARAM_PASSWORD
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.db.dao.case_dao import CaseDAO
from cts_framework.db.dao.http_request_dao import HttpRequestDAO
from cts_framework.db.dao.message_dao import MessageDAO
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.tests_managing.tests_manager import TestsManager


class MetadataReportAction(Action):
    RSD_CHOICES = ('2_5',)
    REDFISH_CHOICES = ('2018_1', '2018_2', '2018_3', '2019_1')

    REPORT_FOLDER = "cts_interoperability_report"
    NEEDED_FILES = ("intel-logo-white.png",)
    HTML_TEMPLATE_DIR = 'template'
    HTML_STATIC = '/'.join((REPORT_FOLDER, 'static'))

    def __init__(self, *params, **kwargs):
        self._logger = LoggingHelper(__name__)
        self.tests_manager = TestsManager()
        Action.__init__(self, *params, **kwargs)

    ACTION = "report"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument('-p', '--preview', default=False, action='store_true',
                                 help='Show report preview in console')
        self.parser.add_argument('--RackScaleMetadata', default='2_5', type=str,
                                 choices=MetadataReportAction.RSD_CHOICES)
        self.parser.add_argument('--RedfishMetadata', default=MetadataReportAction.REDFISH_CHOICES[-1], type=str,
                                 choices=MetadataReportAction.REDFISH_CHOICES)
        self.parser.add_argument('--include_reports', type=lambda l: [int(element) for element in l.split(',')],
                                 default=[],
                                 help='Include additional reports that have been made with Redfish Metadata'
                                      'example: --include_reports 2,10')

    def __create_dirs(self):
        try:
            import pip
            dist = None
            for d in pip.get_installed_distributions():
                if d.project_name == 'Flask-Bootstrap4':
                    dist = d.location

            project_path = '/'.join([dist, 'flask_bootstrap', 'static'])
            copytree(project_path, self.HTML_STATIC)
        except ImportError:
            print('CTS_ERROR:: %s' % ImportError)
        except TypeError:
            print('Missing additional package: Flask-Bootstrap4==4.0.2\nReport will be dumped without proper style.')
        except OSError:
            pass

        try:
            for f in self.NEEDED_FILES:
                if not exists('/'.join((self.HTML_STATIC, f))):
                    resource_package = __name__
                    resource_path = '/'.join((self.HTML_TEMPLATE_DIR, f))

                    template = pkg_resources.resource_string(resource_package, resource_path)
                    with open('/'.join((self.HTML_STATIC, f)), 'w') as resource:
                        resource.write(template)

        except IOError:
            print("CTS can't create file or directory. Do you've appropriate permissions?")
            return False
        return True

    def process_action(self, configuration):
        print "Using CTS in version %s" % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True))
        print "\nComparing...\n\n"

        if not self.__create_dirs():
            exit(1)  # if you can not create a dirs dont create a report

        include_reports = set(configuration.include_reports)
        if include_reports:
            reports_details = self.get_information_about_test_case_from_db(include_reports)
        else:
            reports_details = {}

        with ErrorMute():
            report = self.report_name(configuration.RackScaleMetadata, configuration.RedfishMetadata)
            comparator = MetadataComparator([configuration.RackScaleMetadata, configuration.RedfishMetadata])
            result = comparator.run()
            if result is not None:
                if result.result == Comparator.EQUAL:
                    print "\n\nMetadata sets {metadata} are equal\n\n".format(metadata=" and ".join(
                        (configuration.RackScaleMetadata, configuration.RedfishMetadata)))
                else:
                    print "\n\nMetadata sets {metadata} differ\n\n".format(metadata=" and ".join(
                        (configuration.RackScaleMetadata, configuration.RedfishMetadata)))

                    if configuration.preview:
                        print result.get_side_by_side(*(configuration.RackScaleMetadata, configuration.RedfishMetadata))

            left_text_container = result.left_ext_file.getvalue().split('\n')
            right_text_container = result.right_ext_file.getvalue().split('\n')

            missing_elements_on_left = self._generate_statistics_from_text_container(left_text_container,
                                                                                     right_text_container)
            missing_elements_on_right = self._generate_statistics_from_text_container(right_text_container,
                                                                                      left_text_container)

            preformat_left = self._update_view_for_more_human_readable(left_text_container)
            preformat_right = self._update_view_for_more_human_readable(right_text_container)

            self._build_html_view(configuration,
                                  {'left': preformat_left,
                                   'right': preformat_right,
                                   'missing_left': missing_elements_on_left,
                                   'missing_right': missing_elements_on_right,
                                   'reports_details': reports_details})
            print "Report has been written to: {name}".format(name=report)

    def get_information_about_test_case_from_db(self, include_reports):
        print("These Test Case reports {} will be included".format(include_reports))
        reports_details = {}
        for run_id in include_reports:
            try:
                configuration = None
                test_case_list = []
                configuration = self._prepare_test_case_list(configuration,
                                                             run_id,
                                                             test_case_list)

                if not test_case_list:
                    print("Report {} does not exits in local CTS database or was not fully completed".format(run_id))
                    continue
                reports_details[run_id] = (configuration, test_case_list)

            except (KeyError, IndexError):
                print("Report {} does not exits in local CTS database".format(run_id))
        return reports_details

    def _prepare_test_case_list(self, configuration, run_id, test_case_list):
        for script in ScriptDAO.get_scripts_execution_ids_for_run(run_id):
            Case = collections.namedtuple('Case', 'status message response')

            cases_execution_ids = CaseDAO.get_cases_execution_ids_for_script_execution(script)
            status, _, _ = ScriptDAO.get_script_execution_statistics(script)

            configuration = MetadataReportAction.get_configuration_for_test_case(run_id)
            if 'RedfishMetadata' not in configuration:
                print('Test report {} is not compatible with Interoperability Report operation.\n'
                      'You can only include a test case done with Redfish Metadata {}'
                      .format(run_id,
                              MetadataReportAction.REDFISH_CHOICES))
                continue

            self.__get_messages_for_case_execution(Case, cases_execution_ids,
                                                   self.__get_raw_responses_from_db(),
                                                   status,
                                                   test_case_list)
        return configuration

    def __get_messages_for_case_execution(self, Case, cases_execution_ids, responses_from_http, status, test_case_list):
        # this is range to get only a test cases. We skip discovery process and API response time
        for case_execution_id in cases_execution_ids[4:-1]:
            case_entity = Case(status=status, message=[], response=[])

            for idx, message in enumerate(MessageDAO.get_messages_for_case_execution(case_execution_id)):
                message_text = message.message_text

                case_entity.message.append(message_text)
                if idx == 0 and message_text.find('Checking'):
                    case_entity.response.append(
                        self._merge_case_entity_and_response(message_text,
                                                             responses_from_http))
            test_case_list.append(case_entity)

    @staticmethod
    def __get_raw_responses_from_db():
        response_id_for_test_case = []
        for msg in MessageDAO.get_messages_for_case_execution(3):
            if msg.message_text.find('request_id') != -1:
                load_message = HttpRequestDAO.get_code_request_response(msg.message_text.replace('request_id=', ''))[2]
                response_id_for_test_case.append(json.loads(load_message))
        http_requests_from_execution = HttpRequestDAO.requests_for_script_execution_id(7)
        responses_from_http = [json.loads(HttpRequestDAO.get_code_request_response(x)[2]) for x in
                               http_requests_from_execution]
        return responses_from_http

    @staticmethod
    def _merge_case_entity_and_response(message, responses):
        separated_msg = message.split(' ')
        index = separated_msg.index('Checking')

        odata_from_msg = separated_msg[index + 1]
        all_responses = [r for r in responses if r['@odata.id'] == unicode(odata_from_msg)]
        if not all_responses:
            return ''
        else:
            return json.dumps(all_responses[0]).split(',')

    @staticmethod
    def get_configuration_for_test_case(run_id):
        script_execution = ScriptDAO.get_script_execution_details(run_id)

        param_dict = {'script_path': script_execution.script_path.split('/')[-5:]}
        for param in script_execution.configuration.split("--"):
            if not param:
                continue

            param_name = param.split(" ")[0]
            if param_name in [PARAM_USER, PARAM_PASSWORD]:
                param_value = "**********"
            else:
                param_value = "".join(param.split(" ")[1:])
            param_dict[param_name] = param_value
        return param_dict

    @staticmethod
    def report_name(rackscale_name, redfish_name):
        return "interoperability_report_RSD_{CTS_VERSION}_vs_REDFISH_{redfish_version}_{timestamp}.html". \
            format(CTS_VERSION=rackscale_name,
                   redfish_version=redfish_name,
                   timestamp=datetime.now().strftime("%Y-%m-%d-%H%M%S"))

    @staticmethod
    def _generate_statistics_from_text_container(view, another_view):
        report_statistic = ReportStatistic()
        for i, element in enumerate(view):
            if element == '?':
                report_statistic.add_one_missing_entities_or_type(another_view[i])
            elif element.find('Property: ') != -1:
                report_statistic.add_one_property()
            elif element.find('Entity: ') != -1:
                report_statistic.add_one_entity()
        return report_statistic

    @staticmethod
    def _update_view_for_more_human_readable(view):
        for i, element in enumerate(view):
            if element == '?':
                view[i] = '<span class="badge badge-danger">Missing</span>'
            elif element == '------------------------------------------------------------':
                view[i] = ''
            elif element.find('string=') != -1:
                view[i] = element.replace('string=', '<span class="badge badge-light">(string)</span>')
            elif element.find('Annotation: ') != -1:
                view[i] = element.replace('Annotation:', '<span class="badge badge-secondary">Annotation:</span>')
            elif element.find('Property: ') != -1:
                view[i] = element.replace('Property:', '<span class="badge badge-dark">Property:</span>')
            elif element.find('Entity: ') != -1:
                view[i] = element.replace('Entity:', '<span class="badge badge-primary">Entity:</span>')
            elif element.find('Not Nullable') != -1:
                view[i] = element.replace('Not Nullable', '<span class="badge badge-info">Not Nullable</span>')
            elif element.find('Nullable') != -1:
                view[i] = element.replace('Nullable', '<span class="badge badge-info">Nullable</span>')
            elif element.find('Type: ') != -1:
                view[i] = element.replace('Type:', '<span class="badge badge-primary">Type:</span>')
            elif element.find('Optional') != -1:
                view[i] = element.replace('Optional', '<span class="badge badge-warning">Optional</span>')
            elif element.find('Required') != -1:
                view[i] = element.replace('Required', '<span class="badge badge-warning">Required</span>')
        return view

    def _build_html_view(self, configuration, result_dict):
        resource_path = '/'.join(('template', 'interoperability_template.html'))
        template_path = pkg_resources.resource_string(__name__, resource_path)

        import os
        html_view = os.path.join(*(MetadataReportAction.REPORT_FOLDER, self.report_name(configuration.RackScaleMetadata,
                                                                                        configuration.RedfishMetadata)))

        Template(template_path).stream(left_metadata=configuration.RackScaleMetadata.replace('_', '.'),
                                       right_metadata=configuration.RedfishMetadata.replace('_', '.'),
                                       entity=result_dict).dump(html_view)

    @staticmethod
    def _check_and_make_path(path):
        try:
            if not exists(path):
                makedirs(path)
        except IOError as ioe:
            print ioe


class ReportStatistic(object):
    def __init__(self):
        self.entities = 0
        self.property = 0
        self.missing_entities = []
        self.missing_type = []

    def add_one_property(self):
        self.property += 1

    def add_one_entity(self):
        self.entities += 1

    def add_one_missing_entities_or_type(self, missing_element):
        if missing_element.find('Entity: ') != -1:
            self.missing_entities.append(missing_element.replace('Entity: ', ''))
        elif missing_element.find('Type: ') != -1:
            self.missing_type.append(missing_element.replace('Type: ', ''))
