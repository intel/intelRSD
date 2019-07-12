# Conformance Test Suite
## **Overview**

The Intel<sup>®</sup> Rack Scale Design (Intel<sup>®</sup> RSD) Conformance Test Suite (CTS) tool verifies the northbound API schema conformance of the Pooled System Management (PSME) / Rack Management Module (RMM) and Pod Manager through automated techniques. The tool can also test some hardware and software parameters.

## **Installation**
*NOTE:*  Intel recommends to use Ubuntu* v16.04

### Install Prerequisites:

 * Uninstall Validation Test Suite (VTS) before proceeding with the Conformance Test Suite (CTS) installation on Debian* based systems:

 *NOTE*: The minimum required operating system (OS) version is Ubuntu* v16.04 (server distribution)
 * On Debian* based systems:
   ```
   `sudo pip uninstall Vts

   sudo apt-get -y update`

   sudo apt-get -y install python-pip python-setuptools git python-lxml python-dev python-pysqlite2
   build-essential libssl-dev libffi-dev libxslt-dev libxml2-dev
   ```
 * On Red Hat\* based systems:
   ```
   dnf update -y

   dnf install -y python-pip python-setuptools git python-lxml python-devel redhat-rpm-config
   ```

 * Python* dependencies:
   ```
   sudo pip install "lxml==3.8.0" "bs4==0.0.1" "beautifulsoup4==4.4.0" "configparser==3.5.0" "requests==2.9.1"
   "tabulate==0.7.5" "sqlalchemy==1.1.11" "simplejson==3.8.1" "rstr==2.2.6" "colorama==0.3.7" "jsonpointer==1.9"
   "pyopenssl==17.0.0"  "ndg-httpsclient==0.4.0" "pyasn1==0.1.9" "pandas==0.19.2"

   sudo pip install "Flask-Bootstrap==3.3.7.1" "Flask-Bootstrap4==4.0.2" --no-dependencies
   ```

* pip (preferred installer program) requirements:

    * Required: pip version `v9.0.3` 
    
    To install pip:

    ```
    pip install --upgrade pip<=10
    ```

    ```
    sudo apt install python-pip
    ```
    *Note:* `'apt'` will prompt the user to upgrade the PIP to the latest version, but do not upgrade it as it will cause PIP it to be upgraded to version greater than pip v9.0.3.

    Intel<sup>®</sup> RSD shall update a minimum version of the pip which will only support v9.0.3.
    ```
    pip install --upgrade pip<=10
    ```

    Use the following script to check the version of pip:
    ```
    pip --version
    ```

    *NOTE:* If you have an older version, use this script to update to the latest version:

    ```
    pip install -U pip
    ```

### Installation from `Pre-built .tar.gz` Archive
To install CTS use the `INSTALL.sh` installation script:

1. Download `tar.gz` package with CTS.

2. Extract `tar.gz` package:
    *NOTE:*  If you recieve a CTS .zip package use the following code to unzip:
  
   ```
   unzip CTS_PACKAGE.zip
   tar -zxvf CTS_PACKAGE.tar.gz
   ```

3. Enter extracted directory:
   ```
    cd CTS_PACKAGE
   ```

4. Type the following command and follow the on-screen instructions:

    ```
    sudo bash INSTALL.sh
    ```

5. (`ADVANCED MODE`) If you want to enable advanced mode, use the command:

    ```
    sudo bash INSTALL.sh --interactive
    ```

### Installation Script (`INSTALL.sh`)
The program accepts these parameters:

| Argument | Description |
|--------------|--------------------|
| `1 -f --full-install` | Install CTS and Tests (<span style="color:red">**WARNING**</SPAN>: this deletes all old files!)|
| `2 -p --pip-install` | Install all of pip's dependencies |
| `3 -a --autocompletion` | Install autocompletion for bash (works only with Ubuntu\*)|
| `6 -u --upgrade` | Upgrade CTS|
| `8 -r --repair` | Repair ownership|
| `9 -P --proxy` | Set proxy|
| `0 -S --system` | Install/Upgrade OS dependencies (Ubuntu only)|
| `U -U --Uninstall` | Uninstall CTS|
| `W -W --WipeAllData` | WIPE ALL DATA and uninstall CTS |
| `-p --proxy` | Set proxy for current installation |
| `-i --interactive` | Show advanced mode |
| `--FactoryInstall` | WIPE ALL DATA and perform a CTS reinstallation |
| `--FactoryUninstall` | WIPE ALL DATA and uninstall CTS (<span style="color:red">**WARNING**</SPAN>: this option has no confirmation) |


### Data Structure of CTS

| Description | Path |
|--|--|
|`CTS database`|`HOME_FOLDER/.cts/db/`|
|`CTS general configuration`|`HOME_DIR/.cts/configuration/`|
|`CTS internal tests`|`HOME_FOLDER/.cts/tests/`|
|`Tests data`|`HOME_FOLDER/.cts/tests_data/`
|`User tests`|`/opt/cts/tests/`|
|`CTS log`| `/var/log/cts/`|

### Test Configuration:


    CTS requires that test configuration is part of every `cts execute` command.
    >**NOTE**:  It is possible to use a single configuration file, but CTS accepts more than one configuration file, which users may find convenient. If multiple files are used, configuration files are linked together and when there is a conflict, the value from the file on the right takes precedence over the value from the file on the left.

    * Create `config_file.ini` with the following keys:

        * **ApiEndpoint** - Endpoint to API in format ip:port
        * **UseSSL** - Defines if CTS shall use http or https protocol to connect to the API (Yes/No)  
                 * **CertificateCertFile and CertificateKeyFile** - Absolute paths to client side pem certificate and key files (if API requires client certificate authorization)
        * **User and Password** - User and Password used by CTS to authorize (if API requires basic authorization)

        >**NOTE**: The above set of keys is mandatory for all tests. They tell CTS how to connect to an API Endpoint.

        Other optional keys are:

        * **IgnoreTypes** - User may specify that entities of certain types should not be validated by CTS. Use a comma to ignore more than one type.

          Example:
        `
        IgnoreTypes=Bios.1.1.0.Bios, TypeToIgnore.1.0.1.TypeToIgnore
        `
        * **MapTypes** - User may declare CTS should use a known type to validate a specific unknown type. Example:
        `
        MapTypes=ComputerSystem.v2_0_0:ComputerSystem.v1_0_0
        `
        * **EventListenerAddr** - This flag is only used for CRUD Events tests. It should represent the IP address and, optionally, a port on the device with CTS running, so that it's reachable from the tested device.
        `
        EventListenerAddr=10.0.1.2:8888
        `
        CTS will use `ComputerSystem.v1_0_0 schema` to validate `ComputerSystem.v2_0_0` entities.

        * **ServiceTypeOverride** - If the service implements more than one service type such as combined Pooled System Management Engine (PSME) and Rack Management Module (RMM), CTS must be informed that entities from both services may exist together on the REST. This prevents CTS from raising errors about unknown RMM types when the PSME test package is executed.

            Possible values are: `PODM_2_5, PSME_2_5, RMM_2_5, SS_2_5`

            Example:
            `
            ServiceTypeOverride=PSME_2_5,RMM_2_5
            `

    * Create `hardware_check_list.ini` with the configuration required by the `hardware_check_list` test script: 
      
      >**NOTE:**  Skip this file if checking hardware requirements is not planned. If you prefer to maintain a single configuration file, add these keys to `config_file.ini` (created above):
            * **`IsPsmePresen`t** - Declare (Yes/No). Rack must have one or more PSME software components.
        * **`PowerEfficiency`** - Declared power efficiency (number < 100).
        * **`FanPositionNumberingConsistent`** - Declare (Yes/No). Service personnel should be able to easily identify the location of a failed fan for replacement. 
          >**NOTE:**  Intel recommends that the fan position location label use the base as 1 and be numbered from left to right or right to left or top to bottom or bottom to top, in each subsystem (rack, drawer, or module).
        * **`PowerSupplyPositionNumberingConsistent`** - Declare (Yes/No). Service personnel should be able to easily identify the location of a power supply failure for replacement.
           >**NOTE:** Intel recommends that the power supply position location label use the base as 1 and be numbered from left to right or top to bottom, in each subsystem (rack, drawer, or module).
        * **`IsRMMNetworkPrivateManagement`** - (Yes/No)
        * **`AreManagementAndProductionNetworksSeparated`** - (Yes/No)
        * **`AreComputeBladesServiceableIndependently`** - (Yes/No)

    * Additional ONLY for PATCH test case:
        *  `IgnoredElements` - Declare an absolute path to the file which contains information about endpoints (or properties) which shall be skipped in a PATCH test case. for More information about this refer to `Additional Functionality -> Ignored Elements`.

## **Basic Usage**
### Browsing Available Test Packages, Test Suites, and Test Cases
* To list all available test packages:

  ```
  cts tests list
  ```

* To filter by package name:

  ```
  cts tests list -p Rack_Scale_2_5_POD_Manager
  ```

* To filter by package and test suite names:

  ```
  cts tests list -p Rack_Scale_2_5_POD_Manager -s required
  ```

* To generate a sample configuration file for test case:

  
  cts tests `generate_config PACKAGE_NAME TEST_SCRIPT_NAME -o output_file_with_configuration.ini`
  

### Execution

* To simply execute all tests for `Rack_Scale_2_5_POD_Manager` validation:

  ```
  cts execute tests Rack_Scale_2_5_POD_Manager--config_files config_file.ini
  ```

* To execute only tests for metadata compliance:

  ```
  cts execute tests `Rack_Scale_2_5_POD_Manager--test_suites required --config_files config_file.ini`
  ```

* To execute only a test validating get responses' compliance with provided metadata:

  ```
  cts execute tests Rack_Scale_2_5_POD_Manager --test_scripts validate_get_responses --config_files config_file.ini
  ```

* To set timeout for each script executed, add the flag `-T --timeout`:

  ```
  cts execute tests Rack_Scale_2_5_POD_Manager --config_files config_file.ini -T timeout_in_seconds
  ```

### Test Results Browsing
* To list all test executions:

  ```
  cts status list
  ```

* To show detailed information about a specified execution:

  ```
  cts status show RUN_ID
  ```

* To delete a result from the CTS database:

  ```
  cts status delete RUN_ID
  ```

* To save results to a file (when the "html" option is selected, a new folder called `cts_report` containing html files will be created in your working directory):

  ```
  cts status dump RUN_ID --output_format [html/csv/text]
  ```

### Additional Options
* Show CTS Version:

  ```
  cts version
  ```

* The `CTS SOS` command prepares a package for easier debugging. Use this command when there is suspension of a CTS crash and want to report an issue related to CTS. This command collects information (CTS logs, network configuration, pip dependencies, etc.) and after running the command the working directory should contain an `sos-report-<date>` folder and an `sos-report-<date>.tar.gz` archive.

  ```
  cts sos
  ```


## **Advanced Usage**

### Using Run List to Execute Multiple Tests with a Single Command

The run list is a mechanism which enables multi-step execution and makes it possible to plan more advanced test scenarios.

>**NOTE**: Execution of a run list is very similar to executing a test script.


```
cts execute run_list run_list_2_5
```


`run_list_2_5` is a test specification prepared by the user. It defines the scope of tests to be executed and the configuration that should be used to run tests.
Below is an example of a run list that can be used to execute all v2.5 tests:

    $ cat run_list_2_5

    [PSME_2_5]
    TEST_PACKAGE = Rack_Scale_2_5_PSME
    TEST_SUITES = required
    TEST_CONFIGS = ./config/psme.ini, ./config/hardware_check_list.ini

    [StorageServices_2_5]
    TEST_PACKAGE = Rack_Scale_2_5_Storage_Services
    TEST_SUITES = required
    TEST_CONFIGS = ./config/storage.ini

    [PODM_2_5]
    TEST_PACKAGE = Rack_Scale_2_5_POD_Manager
    TEST_SUITES = required
    TEST_CONFIGS = ./config/podm.ini, ./config/hardware_check_list.ini

    [RMM_2_5]
    TEST_PACKAGE = Rack_Scale_2_5_RMM
    TEST_SUITES = required
    TEST_CONFIGS = ./config/rmm.ini, ./config/hardware_check_list.ini

The `run list` definition refers to additional configuration files:

     $ cat config/psme.ini
    [PSME]
    ApiEndpoint = <IP:PORT>
    UseSSL = Yes
    User = admin
    Password = admin

    $ cat config/storage.ini
    [StorageServices]
    ApiEndpoint = <IP:PORT>
    UseSSL = Yes
    User = admin
    Password = admin

    $ cat config/podm.ini
    [PSME]
    ApiEndpoint = <IP:PORT>
    UseSSL = Yes
    User = admin
    Password = admin

    $ cat config/rmm.ini
    [RMM]
    ApiEndpoint = <IP:PORT>
    UseSSL = Yes
    User = admin
    Password = admin

    $ cat config/hardware_check_list.ini
    [HardwareCheckList]
    PowerEfficiency = 90
    FanPositionNumberingConsistent = Yes
    PowerSupplyPositionNumberingConsistent = Yes
    IsRMMNetworkPrivateManagement = Yes
    AreManagementAndProductionNetworksSeparated = Yes

## **Test Description**

### API Get Validation

Run the API Get Validation tests by passing a flag:

```
--test_scripts validate_get_responses
```

The test is read-only and checks that resources exposed on the REST service are compliant with metadata as part of the Intel<sup>®</sup> RSD specification. CTS raises an error if any of the following conditions occur:

* Property defined as 'required' is not present
* Unknown property is detected in an instance of type that does not allow additional properties
* Value of a property has incorrect type
* Value of a property has a value out of range (resulting from type itself or when min/max values are defined)
* Value of a property does not match a defined regular expression (if any are defined in metadata)
* Resource or object of an unknown type is detected


>**NOTE**: API Get Validation is the most basic test that is available in CTS. Therefore, it should be executed as the first test, since API correctness is a prerequisite for the rest of the tests.

### API Patch Validation

Run the API Patch Validation tests by passing a flag:

```
--test_scripts validate_patch_responses
```

The test iterates through all resources discovered on the REST API in search of patchable properties (that is, properties with `OData.Permissions/ReadWrite` annotation declared in metadata). Based on the property definition, CTS generates a new value and issues a PATCH request followed by GET for verification purposes. If return codes and verification results are correct, the test case passes. Otherwise, CTS reports a **`WARNING`**: `When CTS finishes with an actual API resource, the original state is restored`.

>**NOTE:**  Intel advises that you run this test immediately after API Get Validation.

### **Hardware Checklist Validation**

Run the Hardware Checklist Validation tests by passing a flag:

```
--test_scripts hardware_check_list
```

The test suite consists of both manual and automatic tests that verify requirements from the Platform Design Specification document are met.

Manual tests require an additional set of configuration parameters to work (refer to the `Test Configuration` section). CTS verifies that values provided by the user conform with the PDS document.

The scope of automatic testing is as follows:

* CTS verifies that at least one compute module is present in the POD
* CTS verifies that at least one Ethernet switch is present in the POD
* CTS verifies that at least one Ethernet-based fabric is present in the POD
* CTS verifies that API endpoint uses secure channel (SSL) to serve API
* CTS checks if client is able to perform computer system reset
* CTS checks that all API chassis resources define location
* CTS checks that chassis hierarchy is consistent
* CTS checks that Power Monitoring is possible at Rack level


### CRUD Tests

Create, Read, Update, and Delete (CRUD) tests by passing a flag:

```
--test_scripts crud_operations
```

The test tries to create an instance of a resource, then checks if it was created correctly. Next, it attempts to patch the resource, rechecks the correctness, again and finally deletes it. The test is supposed to clean up the changes it made no matter when it comes to a stop (for example, if the service created resources incorrectly, it skips the patch but still tries to delete it). The following resources are tested:

* PSME: VLAN network interface (without patching - not supported by the REST API)
* Storage Services: Logical Drive, Remote Target (for either LVM or CEPH storage)

### Architecture Specification tests
This test was prepared to increase the automation of the conformance process. After launching this module, the tool will check if the tested service (ex PSME) meets the requirements described in the "Architecture Specification" document. 
Example command to run a test for PSME’ service

```bash
cts execute tests Rack_Scale_2_5_Storage_Services -t arch_spec_validation_ss -c your_config.ini
```


### Rack Scale & OCP Profile test
CTS is able to verify, that specific service met requirements described by Open Compute Project (aka OCP Profiles). One test is enough to verify all resources for all profiles. By default, the tool has 3 profiles included (1. OCPBaselineHardwareManagement, 2. IntelRSDComputeBase, 3. IntelRSDBaselineCompute). All of these profiles user is able to find at ~/.cts/profiles directory. 
Example command to run a profile verification:

```bash
cts execute tests Rack_Scale_2_5_Profiles -c your_config.ini
```

Limitations: 
The tool skips element “ConditionalRequirements” and requires manual verification by the user. “WriteRequirements” are checked for occurrence on the REST API tree, the tool will not send a PATCH query to verify that the resource has been overwritten. This situation can be done with PATCH tests case. Limits make the pass condition conditional



### Preview of Use Case Framework

"Use Case" is a new framework for CTS, which allows the user to easily define new test cases without programing knowledge.

>**Note**: This functionality is in preview stage. Behavior of some features may change in the future.

#### Introduction to Use Case

The description of the new case is via special format of JSON's file.
Example below show how to user can define case for restart "any" Computer System

```json
{
  "name": "Restart",
  "type": "usecase",
  "actions": [
    {
      "name": "Try restart",
      "type": "step",
      "description": "Try to Reset Computer System",
      "action": {
        "username": "admin",
        "password": "admin",
        "request": "{ComputerSystem.v1_0_0.ComputerSystem}/{any}/Actions/ComputerSystem.Reset",
        "request_method": "POST",
        "payload": "{'ResetType': 'ForceRestart'}",
        "response": [
          200,
          204
        ]
      }
    },
    {
      "name": "timeout",
      "type": "timeout",
      "value": 10
    },
    {
      "name": "Try get information",
      "type": "step",
      "description": "Try get information from Computer System",
      "action": {
        "username": "admin",
        "password": "admin",
        "request": "{ComputerSystem.v1_0_0.ComputerSystem}/{any}/Actions/ComputerSystem.Reset",
        "request_method": "GET",
        "payload": null,
        "response": [
          200,
          201
        ],
        "retry_if_fail": "3",
        "retry_timeout": "90"
      }
    }
  ]
}
```

As shown the Use Case is made of three Actions (two step and one timeout).

Code below describe Use Case, this is header. 
```json
{
  "name": "Restart",
  "type": "usecase",
  "actions": []
}
```

In header's field "action", Use Case stores individual activities.
The first step is responsible for finding and restart any of the Computer System and checking if it contains the action.
Verification is done with the help of status code.

```json
{
    "name": "Try restart",
    "type": "step",
    "description": "Try to Reset Computer System",
    "action": {
        "username": "admin",
        "password": "admin",
        "request": "{ComputerSystem.v1_0_0.ComputerSystem}/{any}/Actions/ComputerSystem.Reset",
        "request_method": "POST",
        "payload": "{'ResetType': 'ForceRestart'}",
        "response": [
            200,
            204
        ]
    }
}
```

The second Action, called `timeout` is responsible for calling a timeout (which take a 10 sec)

```json
    {
      "name": "timeout",
      "type": "timeout",
      "value": 10
    }
```

The third action and last in this example will try to `GET` information about Computer System, which was restarted

```json
    {
      "name": "Try get information",
      "type": "step",
      "description": "Try get information from Computer System",
      "action": {
        "username": "admin",
        "password": "admin",
        "request": "{ComputerSystem.v1_0_0.ComputerSystem}/{any}/Actions/ComputerSystem.Reset",
        "request_method": "GET",
        "payload": null,
        "response": [
          200,
          201
        ],
        "retry_if_fail": "3",
        "retry_timeout": "90"
      }
    }
```

#### Type of Actions
In Use Case Framework user is able to build a new Use Case using a few Actions

##### Step

| Name of field | Description |
|--|--|
| `"name"` |  Provide a name to identify Action  |
| `"type"` | For this type of Action provide "step" |
| `"description"` | Displays longer text in the program logs |
| `"action"` | Contain detailed information about Action |
| `"action:username"` | Username of the user with permissions to perform the action |
| `"action:password"` | Password of the user with permissions to perform the action |
| `"action:request"` | URI for specific endpoint or if user want to take action on unspecified resource, user can provide `{odata_type}/{id}/Actions/CompoterSystem.Reset` - where field `{id}` can be replaced with real id of resource or can provide phrase `{any}` that the program should be chosen for him |
| `"action:request_method"` | Specify a method which shall be used in request. You can use one of: `GET`, `POST`, `PATCH` and `DELETE` |
| `"action:payload"` | User is able to attach additional payload with request. Can be `null` |
| `"action:response"` | Use Case Framework will compare the status from response code with this one received from the server. The test case will be passed if the framework finds matching status code |
| `Optional "action:retry_if_fail"` | The parameter is responsible for defining retries if the response from the service is different than the one declared |
| `Optional "action:retry_timeout"` | The parameter is responsible for defining timeout between retries if the response from the website is different than the one declared |

#### Timeout

| Name of field | Description |
|--|--|
| `"name"` | Provide a name to identify Action |
| `"type"` | Must be a timeout |
| `"value"`| Value of timeoout in seconds |


## **Additional Functionality**

### Ignored Elements

The tool has the ability to ignore certain elements while performing patch tests.
This option allows omitting the entire endpoint, or only certain keys.
To achieve this, add additional parameter `IgnoredElements` with an absolute path to a file containing elements to be omitted into the configuration file.
To ignore entire endpoint provide `odata_id` separated with `::` and provide `[*]` to ignore entire endpoint. To ignore a specific subkey on endpoint, use the arrow format `->`.

```text
{odata_id}::[*]
```

or to ignore only a selected elements, put it into `[]` separated by `,`:

```text
{odata_id}::[Key1,Key2]
```

You can also point a specific key, which shall be ignored. For this case, please use arrow format to point key:

```text
{odata_id}::[Key1->SubKey1]
```

Example structure of the file, where the first line ignores one selected element, second line ignores an entire endpoint, and third line ignore only a specific subkey:

```text
/redfish/v1/UpdateService::[ServiceEnabled]
/redfish/v1/TaskService::[*]
/redfish/v1/Systems/1/NetworkInterfaces/1/NetworkDeviceFunctions/1::[Ethernet->MACAddress]

```

When the CTS detects this element (or key), the tool raise a proper message and results of this test case will be ended with `PASSED WITH WARNINGS` status.


### Metadata diff

CTS can be used for testing API compliance as well as comparing metadata. The comparison report is
saved in a text file as shown below:

    $ cts metadata diff --help
    usage: cts diff [-h] [-q [QUALIFIERS [QUALIFIERS ...]]] METADATA METADATA

    positional arguments:
      METADATA              can be any of:
                            - configuration file with remote endpoint definition
                            - Intel<sup>®</sup> RSD release
                              Possible values are:
                            PODM_2_5, PSME_2_5, RMM_2_5, SS_2_5
                            - path to directory that holds metadata xml files

    optional arguments:
      -h, --help            show this help message and exit
      -q [QUALIFIERS [QUALIFIERS ...]], --qualifiers [QUALIFIERS [QUALIFIERS ...]]

### Interoperability Report
CTS can be used to find differences between Rack Scale and Redfish  Metadata.
Since Intel<sup>®</sup> RSD v2.5, the tool includes Redfish v2018_1, v2018_2, v2018_3, v2019.1 and Swordfish v1.0.6 (v1.0.7 is available for v2018.3 and v2019.1) metadata and can generate a report about interoperability.

```bash
cts metadata report -h
```

```bash
usage: cts report [-h] [-p] [--RackScaleMetadata {2_5}]
                  [--RedfishMetadata {2018_1,2018_2,2018_3,2019_1}]
                  [--include_reports INCLUDE_REPORTS]

optional arguments:
  -h, --help            show this help message and exit
  -p, --preview         Show report preview in console
  --RackScaleMetadata {2_5}
  --RedfishMetadata {2018_1,2018_2,2018_3,2019_1}
  --include_reports INCLUDE_REPORTS
                        Include additional reports that have been made with Redfish Metadata
                        example: --include_reports 2,10

```

To generate a report based on the newest version included in CTS, type:


```bash
$ cts metadata report
```

If you want to choose a different version of metadata, for this example Intel<sup>®</sup> RSD 2.5 Redfish v2018.1 use arguments:

```bash
$ cts metadata --RackScaleMetadata 2_5 --RedfishMetadata 2019_1
```

To provide a full spectrum of information, `"Redfish Metadata"` (v2018.1, v2018.2, v2018.3 and v2019.1) includes Swordfish files in v1.0.6 and v1.0.7 (for v2018.3 and above).


#### Include additional information
Into Interoperability report we can add additional information based on the test cases done before (test case shall be done with Redfish Metadata).

To generate such a report please provide `--include_reports {report_id}`. Check a list of all test cases by typing `cts status list`.
```bash
cts metadata report --include_reports {report_id}
```

## **Troubleshooting**
### Issue 1
* When I ran INSTALL.sh I get:
  ```
  INSTALL.sh: line 317: cts: command not found
  ```
### Solution
  There may be an error related to the pip install process. Reinstall pip's CTS package manually with this command:
  ```
  sudo pip install CTS-{version-number}-py2-none-any.whl
  ```

  Where _{version-number}_ is for example: `CTS-2.5.1.0.47.0-py2-none-any.whl`

  If behind a proxy, add additional parameters:

  ```
  sudo pip install --proxy http://{proxy-url}:{proxy-port} CTS-{version-number}-py2-none-any.whl
  ```

  Where `{proxy-url}` and `{proxy-port}` depend on your network settings.

### Issue 2
* I ran a test (with the specified `--test_scripts`), but nothing happened.

### Solution

  ```
  $ cts execute tests Rack_Scale_2_5_POD_Manager --test_scripts validate_patch_responses -c config_files.ini
  ```

Using CTS in v2.5, no scripts where selected to execute.


  The CTS files in `~/.cts/`may have been created using a root account and their ownership has to be changed:

  ```
  [sudo] chown -R USER_NAME ~/.cts/
  [sudo] chmod a+x -R ~/.cts/tests/
  ```

### Issue 3
* I got an error `IOError`, when I ran a test.

### Solution

  The CTS directory `~/.cts/` may have been created using a root account and its ownership has to be changed:

  ```
  [sudo] chown -R USER_NAME ~/.cts
  ```
### Issue 4
* Pip doesn't install CTS's dependencies

### Solution

  The pip may need proxy configuration. Try setting the proxy within the installer using the '9' option during interactive installation.

## **New Features**

The Intel<sup>®</sup> RSD v2.5 (CTS_2.5) release introduces the following features:

1.	New test category `“Rack Scale & OCP Profiles”`. In the new CTS v2.5, the tool can verify profiles (Open Compute Project (OCP) Compute profile, and two Intel<sup>®</sup> RSD specific profiles were implemented).
2.	New test category `“Architecture Specification tests”`. More automation was added to check the compatibility of the tested service with the Archeo document.
3.	CTS v2.5 brings extended mechanisms for ignoring (`“IgnoreType”` and `“IgnoredElements”`) specific elements presented in REST APIs.
>*Note*:	This change applies for PATCH test cases.
4.	Support for Redfish URI’s add notation in the CTS’s GET test case.  
      a.	Rack Scale Metadata and Tests Packages were updated for v2.5.  
      b.	The CTS includes a new version of the Redfish metadata (v2018.3a and 2019.1 with Swordfish v1.0.7) refer to the Redfish metadata provides the following information:    
          1.	Extended information about errors and messages.    
          2.	Improved view of HTML report (Refer to regular test and Intel<sup>®</sup> RSD Interoperability Report)  
          3.	Stability improvements.  
5.	Minor bug fixes.

Intel<sup>®</sup> RSD v2.4. (`CTS_2.4.0.172`) release introduces the following features:

1. Preview of a new test package `Rack_Scale_2_4_Scenario`
2. Performance measurement for GET all test cases (return information about delay for each response from REST API server).
3. New metadata of Intel<sup>®</sup> Rack Scale Design v2.4 and Redfish Metadata v2018.1, v2018.2 and v2018.3 (with Swordfish v1.0.6) are included. Additional information in report can be included based on the test cases done using Redfish Metadata.
4. Generation of Interoperability between Intel<sup>®</sup> Rack Scale (v2.5) and Redfish (v2018.1, v2018.2, and v2018.3 with Swordfish v1.0.6) are available.
5. In all PATCH tests user is able to ignore elements (entire endpoints or just some keys listed on specific endpoint).
6. Stability improvements
7. Minor bug fixes

Intel<sup>®</sup> RSD v2.3. (`CTS_2.3.0.95.0`) release introduces the following features:

1.	New CRUD test for Storage Service (`crud_operations.py`) has been introduced.
2.	Stability improvements
3.	Minor bug fixes

The following list describes all the limitations for this release. Described limitations are targeted for future releases:

* The CTS package does not contain test and metadata packages for older versions of Intel<sup>®</sup> RSD.
* The CRUD for Storage Services does not support NVMe* operations. The CRUD test case for Storage Services is based on a previous software version (Refer to Intel<sup>®</sup> RSD v2.1).

## **Limitations**
>**NOTE**: Intel<sup>®</sup> RSD v2.3 introduces support for Non-volatile Memory Express (NVMe*) technology, but our CRUD test case for Storage Services does not support NVMe* operations. The CRUD test case for Storage Services is based on a previous software version (Refer to Intel<sup>®</sup> RSD v2.1).

## **Known Issues**

### Status Definitions

The table below lists and describes the status of known issues in this release.

| Status| Description |
|--|--|
|Under investigation |The sighting is being investigated.|
|Root cause identified|The root cause of the defect is identified.|
|Workaround available|A temporary solution to the defect is provided until the bug is fixed.|
|As designed |The issue reported is not a defect and the behavior will not be modified.|
|Closed no repro|The situation is not observed anymore and no further investigation is scheduled.|
|Fixed| The defect has been fixed.|

### Known Issues

The tables below present problems and issues found during the testing of this release.
