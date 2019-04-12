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
from __future__ import division

import time

from os import makedirs
from os.path import exists


from cts_core.commons.error import cts_message, cts_error
from urlparse import urlsplit

# TODO break if you get from service 404
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import ReturnCodes
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.helpers.vars.env_consts import Constants


def performance_measure(func):
    def inner(self, *args, **kwargs):
        start = time.time()
        ret = func(self, *args, **kwargs)

        #  return relative path to object, not a full url with schema
        path = urlsplit(args[0])[2]
        self._add_duration_for_object(path,
                                      (time.time() - start) * 1000,
                                      'GET',  # request method
                                      ret[3] if ret[3] is not None else '404')  # status code
        return ret
    return inner


class PerformanceInformation:
    __slots__ = ['_time_container', '_entity_len', '_status', '_statistics']

    def __init__(self, time_container):
        self._time_container = time_container
        self._entity_len = len(time_container)

        self._status = ValidationStatus.PASSED
        self._statistics = {}

    @property
    def get_status(self):
        return self._status

    def calculate_statics(self):
        self._statistics['requests'] = len(self._time_container)

        try:
            self._statistics['latency'] = sorted([float(x.get_duration) for x in self._time_container])

            self._statistics['total_duration'] = sum(self._statistics['latency'])
            self._statistics['average_time'] = self._statistics['total_duration'] / self._statistics['requests']

            self._statistics['status_codes'] = self.__collect_status_codes(self._time_container)
            self._statistics['success_rate'] = self.__measure_success_rate(self._statistics['status_codes'],
                                                                           positive_codes=[
                                                                               ReturnCodes.OK,
                                                                               ReturnCodes.CREATED,
                                                                               ReturnCodes.ACCEPTED,
                                                                               ReturnCodes.NO_CONTENT])

            self._statistics['average_time_unit'], self._statistics['total_duration_unit'] = \
                Constants.TIME_MS, Constants.TIME_MS
            self._statistics['latency50'], self._statistics['latency95'], self._statistics['latency99'], \
            self._statistics['latency_max'] = \
                self._get_percentile_from_latencies(self._statistics['latency'])
        except:
            cts_error("Time container error")

    @staticmethod
    def _get_percentile_from_latencies(latencies_list):
        number_of_elements = len(latencies_list)
        latencies = []
        for multi in [0.5, 0.95, 0.98]:
            latencies.append(latencies_list[int(number_of_elements*multi)])
        latencies.append(latencies_list[-1])
        return latencies

    def print_statistics(self):
        self.calculate_statics()

        # TODO verify that keys are exist
        try:
            cts_message('')
            cts_message('Average time: \t{time} [{unit}]'.format(time=self._statistics['average_time'],
                                                                 unit=self._statistics['average_time_unit']))
            cts_message('Total duration:{time} [{unit}]'.format(time=self._statistics['total_duration'],
                                                                unit=self._statistics['total_duration_unit']))
            cts_message('Latencies [50, 95, 99, max]: [{}, {}, {}, {}] ms'.format(
                self._statistics['latency50'],
                self._statistics['latency95'],
                self._statistics['latency99'],
                self._statistics['latency_max']
            ))
            cts_message('Success rate: \t{percent}%'.format(percent=float(self._statistics['success_rate'])*100))
            cts_message('Status codes: \t{codes}'.
                        format(codes=["{key}: {val}".format(key=k, val=v) for k, v
                                      in self._statistics['status_codes'].iteritems()]))
        except:
            cts_error("Can not show statistics")

    @staticmethod
    def __collect_status_codes(time_container):
        status_codes = [code.get_status for code in time_container]
        return {key: status_codes.count(key) for key in set(status_codes)}

    def __measure_success_rate(self, data, positive_codes):
        total_count = self.__total_count(data)
        only_positive = self.__total_count({i[0]: i[1] for i in data.iteritems() if int(i[0]) in positive_codes})

        if total_count == 0:
            return 0
        return '{0:.4f}'.format(round((only_positive/total_count), 4))

    @staticmethod
    def __total_count(data):
        return sum([e for e in data.values()])

    def print_report(self):
        for enu, element in enumerate(self._time_container, 1):
            cts_message("[{}/{}] {} : {}".format(enu,
                                                 self._entity_len,
                                                 element.get_request_method,
                                                 element.get_object_url))
            cts_message("Status code:\t{}".format(element.get_status))
            cts_message("Duration: \t{} ms\n".format(element.get_duration))

        self.calculate_statics()

    def show_performance_data(self):
        self.print_report()
        self.print_statistics()

    def generate_csv_performance_report(self):
        performance_report = []

        if not exists(Constants.DIR_HOME_CTS_PERFORMANCE_DATA):
            makedirs(Constants.DIR_HOME_CTS_PERFORMANCE_DATA)

        for element in self._time_container:
            performance_report.append(','.join((element.get_request_method,
                                                element.get_status,
                                                element.get_duration,
                                                element.get_object_url)))
        try:
            test_run_id = str(RunDAO.list_all_runs()[-1].id)
        except IndexError:
            test_run_id = '1'
        with open('/'.join((Constants.DIR_HOME_CTS_PERFORMANCE_DATA, test_run_id)) + ".csv", 'w') as report:
            report.write('\n'.join(performance_report))


class PerformanceData:
    __slots__ = ['_object_url', '_duration', '_request_method', '_request_status']

    def __init__(self, object_url, duration, request_method, status):
        self._object_url = object_url
        self._duration = duration
        self._request_method = request_method
        self._request_status = status

    def __str__(self):
        return "{req_status} {duration} {req_method} {object_url}".format(
            req_status=self._request_status,
            duration=self._duration,
            req_method=self._request_method,
            object_url=self._object_url
        )

    def __repr__(self):
        return "PerformanceData(object_url={}, duration={}, request_method={}, status={}".format(
            self._object_url,
            self._duration,
            self._request_method,
            self._request_status
        )

    @property
    def get_status(self):
        return str(self._request_status)

    @property
    def get_duration(self):
        return str(self._duration)

    @property
    def get_object_url(self):
        return str(self._object_url)

    @property
    def get_request_method(self):
        return str(self._request_method)
