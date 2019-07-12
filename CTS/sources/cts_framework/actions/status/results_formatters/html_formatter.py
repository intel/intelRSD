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
from cts_framework.actions.status.results_formatters.formatter import Formatter
from cts_framework.build_information import BuildInformation
from cts_framework.commons.enums import LoggingLevel
from cts_framework.db.dao.case_dao import CaseDAO
from cts_framework.db.dao.message_dao import MessageDAO
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.helpers.python.message_level import MessageLevel
from cts_framework.helpers.python.result_status import ResultStatus

from os import makedirs
from os.path import exists
import pkg_resources
import html_template as html
from distutils.dir_util import copy_tree as copytree


class HtmlFormatter(Formatter):
    REPORT_FOLDER = "cts_report"
    PASSED = "PASSED"
    FAILED = "FAILED"

    HEADER = html.HEADER
    FOOTER = html.FOOTER
    RESULT_RUN_REPORT = html.RESULT_RUN_REPORT
    RESULT_SCRIPT_REPORT = html.RESULT_SCRIPT_REPORT
    RESULT_SAVE_CASE_RESULT = html.RESULT_SAVE_CASE_RESULT

    def __init__(self):
        NEEDED_FILES = ("scripts.js", "style.css", "intel-logo-white.png")
        HTML_TEMPLATE_DIR = 'html_static_files'
        HTML_STATIC = '/'.join((HtmlFormatter.REPORT_FOLDER, 'static'))
        self._check_and_make_path(HTML_STATIC)

        try:
            import pip
            dist = None
            for d in pip.get_installed_distributions():
                if d.project_name == 'Flask-Bootstrap':
                    dist = d.location

            project_path = "/".join([dist, "flask_bootstrap", "static"])
            copytree(project_path, HTML_STATIC)
        except ImportError:
            print "CTS_ERROR:: %s" % ImportError

        try:
            for f in NEEDED_FILES:
                if not exists('/'.join((HTML_STATIC, f))):
                    resource_package = __name__
                    resource_path = '/'.join((HTML_TEMPLATE_DIR, f))

                    template = pkg_resources.resource_string(resource_package, resource_path)
                    with open('/'.join((HTML_STATIC, f)), 'w') as resource:
                        resource.write(template)

        except IOError as e:
            print("CTS can't create file or directory. Do you've appropriate permissions?")

    @staticmethod
    def _set_active_or_disabled_button(count):
        return " active" if count else " disabled"

    @staticmethod
    def _check_and_make_path(path):
        try:
            if not exists(path):
                makedirs(path)
        except IOError as ioe:
            print ioe

    @staticmethod
    def _parent_level(lvl):
        if lvl > 0:
            return "../" * lvl
        else:
            return "./"

    def save_results(self, run_id):
        status, passed, total = RunDAO.get_overall_run_result(run_id)
        text_status = HtmlFormatter.PASSED if status else HtmlFormatter.FAILED

        script_execution_ids = ScriptDAO.get_scripts_execution_ids_for_run(run_id)

        test_scripts_result = ""
        for script_execution_id in script_execution_ids:
            script_execution_details = ScriptDAO.get_script_execution_details(script_execution_id)

            self.save_script_results(script_execution_id, run_id)

            overall, passed, total = ScriptDAO.get_script_execution_statistics(script_execution_id)
            if passed == total:
                li_style = "success"
                glyphicon = "ok"
            elif overall:
                li_style = "warning"
                glyphicon = "exclamation-sign"
            else:
                li_style = "danger"
                glyphicon = "remove"

            test_scripts_result += "<li class='list-group-item list-item-%s'><a class='report' " \
                                   "href='%s_script_report/%s_script_report.html'><span class='report glyphicon " \
                                   "glyphicon-%s'></span>  %s</a></li>" % (
                                       li_style, script_execution_details.id, script_execution_details.id, glyphicon,
                                       script_execution_details.script_path)

        result_report = HtmlFormatter.RESULT_RUN_REPORT.format(status=text_status, passed=passed, total=total,
                                                               test_script_result=test_scripts_result)

        case_results_dir = '/'.join((HtmlFormatter.REPORT_FOLDER,
                                     "%s_run_report" % run_id))
        case_results_html = '/'.join((case_results_dir,
                                      "%s_run_report.html" % run_id))
        self._check_and_make_path(case_results_dir)
        parent_lvl = self._parent_level(1)

        with open(case_results_html, "w") as report:
            report.write("{header}{resultRunReport}{footer}".format(
                header=HtmlFormatter.HEADER.format(parent_level=parent_lvl),
                resultRunReport=result_report,
                footer=HtmlFormatter.FOOTER.format(version=BuildInformation.get_version(), parent_level=parent_lvl)))

    def save_script_results(self, script_execution_id, run_id):
        cases_execution_ids = CaseDAO.get_cases_execution_ids_for_script_execution(script_execution_id)
        status, passed, total = ScriptDAO.get_script_execution_statistics(script_execution_id)

        text_status = HtmlFormatter.PASSED if status else HtmlFormatter.FAILED
        test_script_result = ""

        for case_execution_id in cases_execution_ids:
            case_execution_details = CaseDAO.get_case_execution_details(case_execution_id)
            if case_execution_details.status == ResultStatus.PASSED:
                li_style = "success"
                glyphicon = "ok"
            elif case_execution_details.status in [ResultStatus.FAILED, ResultStatus.TIMEOUT]:
                li_style = "warning"
                glyphicon = "exclamation-sign"
            elif case_execution_details.status in [ResultStatus.BLOCKED, ResultStatus.UNKNOWN]:
                li_style = "danger"
                glyphicon = "remove"
            else:
                li_style = "neutral"
                glyphicon = "minus"

            test_script_result += "<li class='list-group-item list-item-%s'><a class='report' " \
                                  "href='case_results/%s_case_results.html'><span class='report glyphicon " \
                                  "glyphicon-%s'></span>  %s</a></li>" % (li_style,
                                                                          case_execution_details.id,
                                                                          glyphicon,
                                                                          case_execution_details.name)

            self.save_case_result(case_execution_id, script_execution_id, run_id)

        result_report = HtmlFormatter.RESULT_SCRIPT_REPORT.format(status=text_status, passed=passed, total=total,
                                                                  test_script_result=test_script_result)

        script_path = '/'.join((HtmlFormatter.REPORT_FOLDER,
                                "%s_run_report" % run_id,
                                "%s_script_report" % script_execution_id))
        script_html_path = '/'.join((script_path,
                                     "%s_script_report.html" % script_execution_id))
        self._check_and_make_path(script_path)
        parent_lvl = self._parent_level(2)

        with open(script_html_path, "w") as report:
            report.write(
                "{header}{resultRunReport}{footer}".format(header=HtmlFormatter.HEADER.format(parent_level=parent_lvl),
                                                           resultRunReport=result_report,
                                                           footer=HtmlFormatter.FOOTER.format(
                                                               version=BuildInformation.get_version(),
                                                               parent_level=parent_lvl)))

    def save_case_result(self, case_execution_id, script_execution_id, run_id):
        result = ""
        status_count = {'neutral': 0, 'debug': 0, 'warning': 0, 'error': 0}
        for message in MessageDAO.get_messages_for_case_execution(case_execution_id):
            if LoggingLevel.CONTROL == message.message_level:
                for (status_code, request, response) in self.request_from_control_channel(message.message_text):
                    li_style = "color: grey"

                    result += "<li class='list-group-item list-item-{li_style}'> <div><br>Request:<br>".format(
                        li_style=li_style)
                    result += "<pre>{message}</pre>\n". \
                        format(message=request.replace("\n", "<br>"))
                    result += "</div>"

                    result += "<div style='{li_style}'><br>Response:<br>".format(li_style=li_style)
                    result += "<pre>{message}</pre>\n". \
                        format(message=response.replace("\n", "<br>"))
                    result += "</div></li>"
            else:
                if message.message_level == MessageLevel.ERROR:
                    status_count['error'] += 1
                    li_style = "danger"
                    glyphicon = "remove"
                elif message.message_level == MessageLevel.WARNING:
                    status_count['warning'] += 1
                    li_style = "warning"
                    glyphicon = "exclamation-sign"
                elif message.message_level == MessageLevel.DEBUG:
                    status_count['debug'] += 1
                    li_style = "neutral"
                    glyphicon = "asterisk"
                else:
                    status_count['neutral'] += 1
                    li_style = "default"
                    glyphicon = "unchecked"

                result += "<li class='list-group-item list-item-{li_style}'><span class='report glyphicon glyphicon-{glyphicon}'></span>{datetime} - {message}</li>\n". \
                    format(li_style=li_style,
                           glyphicon=glyphicon,
                           datetime=message.message_datetime,
                           message=message.message_text.replace("\n", "<br>"))

        result_report = HtmlFormatter.RESULT_SAVE_CASE_RESULT.format(test_script_result=result,
                                                                     successBadge=status_count['neutral'],
                                                                     debugBadge=status_count['debug'],
                                                                     warningsBadge=status_count['warning'],
                                                                     errorsBadge=status_count['error'],
                                                                     successBadgeStatus=self._set_active_or_disabled_button(
                                                                         status_count['neutral']),
                                                                     debugBadgeStatus=self._set_active_or_disabled_button(
                                                                         status_count['debug']),
                                                                     warningsBadgeStatus=self._set_active_or_disabled_button(
                                                                         status_count['warning']),
                                                                     errorsBadgeStatus=self._set_active_or_disabled_button(
                                                                         status_count['error']))

        case_results_dir = '/'.join((HtmlFormatter.REPORT_FOLDER,
                                     "%s_run_report" % run_id,
                                     "%s_script_report" % script_execution_id,
                                     "case_results"))
        case_results_html = '/'.join((case_results_dir,
                                      "%s_case_results.html" % case_execution_id))
        self._check_and_make_path(case_results_dir)
        parent_lvl = self._parent_level(3)

        with open(case_results_html, "w") as report:
            report.write("{header}{resultRunReport}{footer}".format(
                header=HtmlFormatter.HEADER.format(parent_level=parent_lvl),
                resultRunReport=result_report,
                footer=HtmlFormatter.FOOTER.format(version=BuildInformation.get_version(), parent_level=parent_lvl)))
