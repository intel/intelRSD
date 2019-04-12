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

HEADER = """
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <link rel="stylesheet" href="{parent_level}static/style.css" />
        <link rel="stylesheet" href="{parent_level}static/css/bootstrap.css">
    </head>

    <body>
        <nav class="navbar navbar-default header">
            <div class="container-fluid header">
                <div class="navbar-header">
                    <a class="navbar-brand" href="#">
                        <img src="{parent_level}static/intel-logo-white.png" alt="Intel" width="50px" />
                    </a>
                </div>
                <h2>Intel Rack Scale Design</h2>
            </div>
        </nav>
        <div class="page-header col-md-12">
            <h1> Intel Conformance Test Suite <small>  report</small></h1>
        </div>
            """

FOOTER = """
        <div class="footer">
            <ol class="breadcrumb">
                <li>Tests were performed using CTS in version </li>
                <li><i>{version}</i></li>
            </ol>
        </div>

    </body>

    <!--Custom scripts-->
    <script src="{parent_level}static/scripts.js"></script>

    </html>
        """

RESULT_RUN_REPORT = """
        <div class="col-md-12">
            <div id="panelHeading" class="panel ">
                <div class="panel-heading">
                    <h2>Overall status <span id="overallStatus"><b>{status}</b></span> </h2>
                </div>

                <h2 class="header-h2">Passing rate  <span id="passingRateBadge" class="badge ">{passed}/{total}</span></h2>

                <div class="header-space">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <h3 class="panel-title">Executed test scripts:</h3>
                        </div>

                        <ul class="list-group">
                            {test_script_result}
                        </ul>
                    </div>
                </div>

            </div>
        </div>
        </div>
    """

RESULT_SCRIPT_REPORT = """
        <div class="col-md-12">
            <!-- This class is managed by script PanelColorChange() -->
            <div id="panelHeading" class="panel">
                <div class="panel-heading">
                    <h2>Overall status <span id="overallStatus"><b>{status}</b></span> </h2>
                </div>

                <h2 class="header-h2">Passing rate <span id="passingRateBadge" class="badge ">{passed}/{total}</span></h2>

                <div class="header-space">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <h3 class="panel-title">Executed test scripts:</h3>
                        </div>

                        <ul class="list-group">
                            {test_script_result}
                        </ul>
                    </div>
                </div>
            </div>
        </div>
    """

RESULT_SAVE_CASE_RESULT = """
        <div class="col-md-12">
            <div id="panelHeading" class="panel">
                <div class="panel-heading">
                    <h2>Overall status <span id="overallStatus"><b></b></span> </h2>
                </div>

                <h2 class="header-h2">
                  <small>
                    <button id="btn-success" name="list-item-default" type="button" class="btn btn-default btn-lg {successBadgeStatus}" onclick="javascript:filter(this.id, this.name)">
                    <span id="successBadge" class="badge">{successBadge}</span> Neutral
                    </button>

                    <button id="btn-debug" name="list-item-neutral" type="button" class="btn btn-default btn-lg {debugBadgeStatus}" onclick="javascript:filter(this.id, this.name)">
                    <span id="debugBadge" class="badge">{debugBadge}</span> Debug
                    </button>

                    <button id="btn-warning" name="list-item-warning" type="button" class="btn btn-default btn-lg {warningsBadgeStatus}" onclick="javascript:filter(this.id, this.name)">
                    <span id="warningBadge" class="badge">{warningsBadge}</span> Warning
                    </button>

                    <button id="btn-error" name="list-item-danger" type="button" class="btn btn-default btn-lg {errorsBadgeStatus}" onclick="javascript:filter(this.id, this.name)">
                    <span id="errorBadge" class="badge">{errorsBadge}</span> Error
                    </button>

                  </small>
                </h2>
                <div class="header-space">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <h3 class="panel-title">Logs from case execution</h3>
                        </div>

                        <ul class="list-group">
                            {test_script_result}
                        </ul>
                    </div>
                </div>
            </div>
        </div>
    """
