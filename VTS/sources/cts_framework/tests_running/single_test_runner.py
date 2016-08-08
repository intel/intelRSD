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

from traceback import format_exc
import Queue
import multiprocessing

from cts_framework.configuration.config_property_reader import ValueNotFound
from cts_framework.configuration.configuration_file_reader import ConfigurationFileReader
from cts_framework.db.dao.test_case_results_dao import TestCaseResultsDAO
from cts_framework.logging.logging_stream import LoggingStream
from cts_framework.tests_helpers import ResultStatus, MessageLevel
from cts_framework.tests_helpers.test_case_flags import TestCaseFatalErrorFlag

GENERAL = "GENERAL"
REPORTS_DIRECTORY = "REPORTS_DIRECTORY"


class SingleTestRunner:
    def __init__(self, test_case):
        """
        :type test_case: cts_framework.tests_helpers.test_case.TestCase
        :param test_case: test case to execute
        """
        self.test_case = test_case

    def run(self):
        test_case_result = TestCaseResultsDAO().get_test_case_result(self.test_case.test_case_result_id)
        logging_configuration = ConfigurationFileReader().read_file()

        logging_stream = LoggingStream(tag="%s.%s.%s" % (self.test_case.test_case_details.tests_package_name,
                                                         self.test_case.test_case_details.tests_suite_name,
                                                         self.test_case.test_case_details.name),
                                       test_case_result_id=test_case_result.test_case_id,
                                       html_file="%s/%s/%s_%s/report.html" %
                                                 (logging_configuration[GENERAL][REPORTS_DIRECTORY],
                                                  test_case_result.test_case_test_run_id, test_case_result.test_case_id,
                                                  self.test_case.test_case_details.name),
                                       csv_file="%s/%s/%s_%s/report.csv" %
                                                (logging_configuration[GENERAL][REPORTS_DIRECTORY],
                                                 test_case_result.test_case_test_run_id, test_case_result.test_case_id,
                                                 self.test_case.test_case_details.name))

        logging_stream.enable_stream()

        queue = multiprocessing.Queue()
        test_case_thread = multiprocessing.Process(target=self._run_test_case, args=(queue,))
        test_case_thread.start()
        try:
            test_case_thread.join()
            status = queue.get(timeout=1)
        except Queue.Empty:
            test_case_thread.terminate()
            self.test_case.set_status(ResultStatus.TIMEOUT)
            status = ResultStatus.TIMEOUT
        logging_stream.disable_stream()

        return status not in [ResultStatus.TIMEOUT, ResultStatus.FAILED]

    def _run_test_case(self, queue):
        """
        :type queue: multiprocessing.Queue
        :param queue: way to return execution status
        """
        self.test_case.set_status(ResultStatus.RUNNING)
        execute_run = True

        try:
            print "%s:: %s" % (MessageLevel.DEBUG, "Starting preparing test case.")
            self.test_case.before_run()
            print "%s:: %s" % (MessageLevel.DEBUG, "Preparing test case finished successfully.")
        except ValueNotFound as err:
            print "%s:: %s" % (MessageLevel.ERROR, err.message)
            self.test_case.set_status(ResultStatus.BLOCKED)
            execute_run = False
        except Exception:
            print "%s:: %s" % (MessageLevel.ERROR, "Unexpected exception during before_run action.")
            self.test_case.set_status(ResultStatus.BLOCKED)
            self.test_case.set_status_failed()
            execute_run = False
        except BaseException:
            execute_run = False
            print "%s:: %s" % (MessageLevel.ERROR,
                               "Unhandled critical exception during test case execution.\n%s" % format_exc())
            self.test_case.set_status_failed()

        if execute_run:
            try:
                print "%s:: %s" % (MessageLevel.DEBUG, "Starting test script.")
                self.test_case.run()
                print "%s:: %s" % (MessageLevel.DEBUG, "Test case finished successfully.")
            except ValueNotFound as err:
                self.test_case.set_status(ResultStatus.BLOCKED)
                print "%s:: %s" % (MessageLevel.ERROR, err.message)
            except TestCaseFatalErrorFlag as err:
                self.test_case.set_status_failed()
                print "%s:: %s" % (MessageLevel.ERROR,
                                   "Test case execution terminated due to fatal error: %s" % err.message)
            except Exception:
                print "%s:: %s" % (MessageLevel.ERROR,
                                   "Unhandled exception during test case execution.\n%s" % format_exc())
                self.test_case.set_status_failed()
            except BaseException:
                print "%s:: %s" % (MessageLevel.ERROR,
                                   "Unhandled critical exception during test case execution.\n%s" % format_exc())
                self.test_case.set_status_failed()

        try:
            print "%s:: %s" % (MessageLevel.DEBUG, "Cleanup after test case.")
            self.test_case.after_run()
            print "%s:: %s" % (MessageLevel.DEBUG, "Clean up finished successfully")
        except ValueNotFound as err:
            print "%s:: %s" % (MessageLevel.ERROR, err.message)
        except Exception:
            print "%s:: %s" % (MessageLevel.ERROR, "Unhandled exception during test case cleanup script.")
        except BaseException:
            print "%s:: %s" % (MessageLevel.ERROR,
                               "Unhandled critical exception during test case execution.\n%s" % format_exc())
            self.test_case.set_status_failed()

        if self.test_case.get_status() == ResultStatus.RUNNING:
            # test case has not set any status
            self.test_case.set_status(ResultStatus.UNKNOWN)

        queue.put(TestCaseResultsDAO().get_test_case_status(self.test_case.test_case_result_id))
