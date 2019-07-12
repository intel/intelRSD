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
import os
import sys

import argparse
from abc import abstractmethod
from traceback import format_exc

from cts_core.commons.error import cts_error
from cts_core.metadata.metadata_manager import MetadataMalformed
from cts_framework.commons.enums import ErrorReturnCodes
from cts_core.commons.api_caller import ApiCaller

def exit(self, status=0, message=None):
    if message:
        cts_error("{message}", message=str(message))
        sys.stdout.flush()
    sys.exit(status)


argparse.ArgumentParser.exit = exit  # Monkey patch to see 'argument error' in the log

from cts_framework.configuration.configuration import ValueNotFound, Configuration
from cts_framework.helpers.python.message_level import MessageLevel
from cts_framework.helpers.python.result_status import ResultStatus
from cts_framework.helpers.python.test_case_flags import TestCaseFatalErrorFlag
from cts_framework.db.database_connection_handler import DatabaseConnectionHandler

unbuffered_stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
sys.stdout = unbuffered_stdout

class TestScript(object):
    """
    base test case class
    """
    TIMEOUT = 60
    """:type: int"""
    DESCRIPTION = "Not specified"
    """:type: str"""
    CONFIGURATION_PARAMETERS = []
    """:type: list[TestCase.ConfigurationParameter]"""

    class ConfigurationParameter:
        def __init__(self, parameter_name, parameter_description, parameter_type, is_required, parameter_default_value,
                     min=None, max=None):
            self.name = parameter_name
            self.description = parameter_description
            self.type = parameter_type
            self.is_required = is_required
            self.default_value = parameter_default_value
            self.min = min
            self.max = max

    def __init__(self, configuration):
        self.configuration = configuration
        DatabaseConnectionHandler.initialize_db_connection_handler()

    @abstractmethod
    def run(self):
        pass

    def before_run(self):
        pass

    def after_run(self):
        pass

    def set_status(self, status):
        print "STATUS::%s" % status

    def set_status_failed(self):
        self.set_status(ResultStatus.FAILED)

    def set_status_passed(self):
        self.set_status(ResultStatus.PASSED)

    def set_status_passed_with_warnings(self):
        self.set_status(ResultStatus.PASSED_WITH_WARNINGS)

    def set_status_blocked(self):
        self.set_status(ResultStatus.BLOCKED)

    def log_message(self, logging_level, message):
        print "%s::%s" % (logging_level, message)

    def log_error(self, message):
        self.log_message(MessageLevel.ERROR, message)

    def log_warning(self, message):
        self.log_message(MessageLevel.WARNING, message)

    def log_info(self, message):
        self.log_message(MessageLevel.MESSAGE, message)

    def log_debug(self, message):
        self.log_message(MessageLevel.DEBUG, message)

    def execute(self):
        print "TEST_CASE::SetUp"
        self.set_status(ResultStatus.RUNNING)
        execute_run = True
        try:
            self.set_status_passed()
            self.before_run()
        except ValueNotFound as err:
            print "%s::%s" % (MessageLevel.ERROR, err.message)
            self.set_status(ResultStatus.BLOCKED)
            execute_run = False
        except Exception:
            cts_error("Exception while executing before_run action: {stack:stacktrace}",
                      stack=format_exc())
            self.set_status(ResultStatus.BLOCKED)
            self.set_status_failed()
            execute_run = False
        except BaseException:
            execute_run = False
            cts_error("Critical exception while executing before_run action: {stack:stacktrace}",
                      stack=format_exc())
            self.set_status_failed()

        if execute_run:
            try:
                self.run()
            except MetadataMalformed as err:
                cts_error("Test case execution blocked due to metadata containing malformed XML")
                self.set_status_failed()
            except ValueNotFound as err:
                self.set_status(ResultStatus.BLOCKED)
                cts_error("Test case execution blocked due to: {err:exception}", err=err)
                cts_error("{stacktrace:stacktrace}", stacktrace=format_exc().replace("\n", "\nERROR::"))
            except (TestCaseFatalErrorFlag, Exception, BaseException) as err:
                self.set_status_failed()
                cts_error("Test case execution terminated due to fatal error: {err:exception}", err=err)
                cts_error("{stacktrace:stacktrace}", stacktrace=format_exc())

        try:
            self.after_run()
        except (ValueNotFound, Exception, BaseException) as err:
            cts_error("Exception during test case cleanup {err:exception}", err=err)
            self.set_status_failed()

    @classmethod
    def service_cli(cls):
        main_parser = argparse.ArgumentParser()
        subparsers = main_parser.add_subparsers()

        description_parser = argparse.ArgumentParser(add_help=False)
        description_parser.set_defaults(**dict(ACTION="description"))
        description_parser.add_argument("parameter", type=str, nargs="?")
        subparsers.add_parser("description", parents=[description_parser])

        parameter_list_parser = argparse.ArgumentParser(add_help=False)
        parameter_list_parser.set_defaults(**dict(ACTION="parameters_list"))
        subparsers.add_parser("parameters_list", parents=[parameter_list_parser])

        execute_parser = argparse.ArgumentParser(add_help=False)
        execute_parser.set_defaults(**dict(ACTION="execute"))

        expected_param_type = {}
        for param in cls.CONFIGURATION_PARAMETERS:
                expected_param_type.update({param.name: param.type})
                execute_parser.add_argument("--%s" % param.name, type=param.type)

        subparsers.add_parser("execute", parents=[execute_parser])
        configuration = main_parser.parse_args()

        cls.__additional_configuration_types_check(configuration, expected_param_type)

        {"description": cls.service_description, "parameters_list": cls.service_parameter_list,
         "execute": cls.service_execute}[configuration.ACTION](configuration)


    @classmethod
    def _check_endpoint_connection(cls, can_run, configuration):
        if can_run:
            service_root = ApiCaller(configuration)
            link, kwargs = service_root._build_request(configuration.ApiEndpoint)
            url = link.link[:link.link.rfind("/")+1] + "redfish/v1"

            response, status_code = service_root._do_request(kwargs, url,"Get")
            if status_code in range(400, 600):
                cts_error("{url:id} Get failed. Status code: {code}", url=url, code=status_code)
                try:
                    cts_error('{code} : {description}. Please check your configuration.', code=status_code,
                              description=ErrorReturnCodes.ErrorCodes[status_code])
                except KeyError:
                    pass
                return False
            else:
                return True
        else:
            return False


    @classmethod
    def __additional_configuration_types_check(cls, configuration, expected_param_type):
        configuration_vars = vars(configuration)
        problems_flag = False
        try:
            for t in configuration_vars:
                if configuration_vars[t] != None:
                    if not isinstance(configuration_vars[t], expected_param_type[t]):
                        cts_error("Issue with config file: %s=%s should be %s is %s" % (
                            t, configuration_vars[t], expected_param_type[t], type(t)))
                        problems_flag = True

                    try:
                        # if conversion fails, get next type to verification
                        con = float(configuration_vars[t])
                        if not isinstance(con, expected_param_type[t]):
                            cts_error("Issue with config file: %s=%s should be %s is %s" % (
                            t, con, expected_param_type[t], type(con)))
                            problems_flag = True
                    except:
                        pass
        except KeyError:
            pass
        finally:
            if problems_flag:
                exit(1)

    @classmethod
    def service_description(cls, configuration):
        test_case_parameter_name = configuration.parameter
        if test_case_parameter_name:
            try:
                print {param.name: "%s::%s" % (param.is_required, param.description) for param in
                       cls.CONFIGURATION_PARAMETERS}[test_case_parameter_name]
            except KeyError:
                print "parameter not found"
        else:
            print cls.DESCRIPTION

    @classmethod
    def service_parameter_list(cls, configuration):
        print ",".join([param.name for param in cls.CONFIGURATION_PARAMETERS])

    @classmethod
    def service_execute(cls, configuration):
        print "TEST_CASE::Checking configuration"
        can_run = True
        namespace_configuration = configuration
        can_run, configuration = cls.sanitize_configuration(can_run, configuration)
        can_run = cls.validate_configuration(can_run, configuration)
        can_run = cls._check_endpoint_connection(can_run, namespace_configuration)

        if can_run:
            print "STATUS::%s" % ResultStatus.PASSED
            cls(configuration).execute()
        else:
            print "STATUS::%s" % ResultStatus.FAILED

    @classmethod
    def sanitize_configuration(cls, can_run, configuration):
        test_case_configuration = Configuration()
        for parameter in cls.CONFIGURATION_PARAMETERS:
            configuration_param_value = getattr(configuration, parameter.name)
            if configuration_param_value is None and parameter.is_required:
                cts_error("Required configuration parameter {name} not defined",
                          name=parameter.name)
                can_run = False
            if configuration_param_value is None and parameter.default_value is not None:
                configuration_param_value = parameter.default_value
            test_case_configuration.update_configuration(**{parameter.name: configuration_param_value})
        return can_run, test_case_configuration

    @classmethod
    def validate_configuration(cls, can_run, configuration):
        for parameter in cls.CONFIGURATION_PARAMETERS:
            configuration_param_value = getattr(configuration, parameter.name)

            if parameter.min is not None:
                if isinstance(configuration_param_value, (int, long, float)):
                    if parameter.type(configuration_param_value) < parameter.min:
                        cts_error("Configuration parameter {name} should be >= {min}",
                                  name=parameter.name, min=parameter.min)
                        can_run = False

            if parameter.max is not None:
                if isinstance(configuration_param_value, (int, long, float)):
                    if parameter.type(configuration_param_value) > parameter.max:
                        cts_error("Configuration parameter {name} should be <= {max}",
                                  name=parameter.name, max=parameter.max)
                        can_run = False

        return can_run

    @classmethod
    def parse(cls, test_script_parser):
        parser = argparse.ArgumentParser(add_help=False)
        for configuration_parameter in cls.CONFIGURATION_PARAMETERS:
            parser.add_argument("--%s" % configuration_parameter.name, type=configuration_parameter.type)

        test_script_parser.add_parser(cls.__class__, parents=[parser])
