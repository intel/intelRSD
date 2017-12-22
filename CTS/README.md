# Conformance Test Suite

## Installation

### Install prerequisites

 * Please uninstall VTS before proceeding with the CTS installation.
    `sudo pip uninstall Vts`

 * On recommended Ubuntu 16.04 system:

    `sudo apt-get -y update`

    `sudo apt-get -y install python-pip python-setuptools git python-lxml python-dev python-pysqlite2
    build-essential libssl-dev libffi-dev libxslt-dev libxml2-dev`

 * On RedHat based systems:

     `dnf update -y`

     `dnf install -y python-pip python-setuptools git python-lxml python-devel redhat-rpm-config`

 * Python dependencies:

    `sudo pip install "lxml==3.8.0" "bs4==0.0.1" "beautifulsoup4==4.4.0" "configparser==3.5.0" "requests==2.9.1"
    "tabulate==0.7.5" "sqlalchemy==1.1.11" "simplejson==3.8.1" "rstr==2.2.6" "colorama==0.3.7" "jsonpointer==1.9" "pyopenssl==17.0.0" "ndg-httpsclient==0.4.0" "pyasn1==0.1.9" "pandas==0.19.2"`

    `sudo pip install "Flask-Bootstrap==3.3.7.1" --no-dependencies`

* PIP requirements

    Check version of PIP:

    `pip --version`

    Minimum requirements for PIP is version 8.0.0. If you have older version, please update to latest one:

    `pip install -U pip`


### Installation from pre-built .tar.gz archive.
To install CTS, please you to use installation script `INSTALL.sh`.

1. Download tar.gz package with CTS

2. Extract tar.gz package:

    `
    tar -zxvf CTS_PACKAGE.tar.gz
    `

3. Enter extracted directory:

    `
    cd CTS_PACKAGE
    `

4. Please, type command and follow the on-screen instructions:

    `
    sudo bash INSTALL.sh
    `

5. (ADVANCED MODE) If you want to enable advanced mode, use:

    `
    sudo bash INSTALL.sh --interactive
    `

### Installation script (INSTALL.sh)
Program accepts these parameters:

| Arguments | Description |
|--------------|--------------------|
| `1 -f --full-install` | Install CTS & Tests (warning! this deletes all old files!)|
| `2 -p --pip-install` | Install all PIP's dependencies |
| `3 -a --autocompletion` | Install autocompletion for bash (works only with Ubuntu)|
| `6 -u --upgrade` | Upgrade CTS|
| `8 -r --repair` | Repair ownership|
| `U -U --Uninstall` | Uninstall CTS|
| `W -W --WipeAllData` | WIPE ALL DATA and uninstall CTS |
| `-i --interactive` | Show advanced mode |
| `--FactoryInstall` | WIPE ALL DATA and perform a CTS reinstallation |
| `--FactoryUninstall` | WIPE ALL DATA and uninstall CTS (WARNING! this option has no confirmation) |


### Data structure of CTS

| Description | Path |
|--|--|
|CTS database|`HOME_FOLDER/.cts/db/`|
|CTS general configuration|`HOME_DIR/.cts/configuration/`|
|CTS internal tests|`HOME_FOLDER/.cts/tests/`|
|Tests data|`HOME_FOLDER/.cts/tests_data/`
|User tests|`/opt/cts/tests/`|
|CTS log| `/var/log/cts/`|

* Test configuration:

    CTS requires that test configuration is part of every `cts execute` command.
    It is possible to use a single configuration file, but CTS accepts more than one configuration file and the user may find it convenient. If used in this way, configuration files are concatenated and in case of conflict, value from file to the right takes precedence over value from file to the left.

    * create config_file.ini with keys:

        *  __ApiEndpoint__ - endpoint to API in format ip:port
        *  __UseSSL__ - defines if CTS shall use http or https protocol to connect to api (Yes/No)
        *  __CertificateCertFile__ and __CertificateKeyFile__ - absolute paths to client side pem certificate and key files (if API requires client certificate authorization)
        *  __User__ and __Password__ - User and Password used by CTS to authorize (if API requires basic authorization)

        The above set of keys is mandatory for all tests and tells the CTS how to connect to API endpoint.
        Other optional keys are:

        *  __IgnoreTypes__ - User may may specify that entities of certain types should not be validated by CTS. Use comma to ignore more than one type. Example:
        `
        IgnoreTypes=Bios.1.1.0.Bios, TypeToIgnore.1.0.1.TypeToIgnore
        `
        *  __MapTypes__ - User may declare that CTS should use known type to validate specific unknown type. Example:
        `
        MapTypes=ComputerSystem.v2_0_0:ComputerSystem.v1_0_0
        `
        *  __EventListenerAddr__ - This flag is only used for CRUD Events tests. It should represent the IP and optionally a port on the device with CTS running, so that it's reachable from the tested device.
        `
        EventListenerAddr=10.0.1.2:8888
        `
        CTS will use ComputerSystem.v1_0_0 schema do validate ComputerSystem.v2_0_0 entities
        *  __ServiceTypeOverride__ - If your service implements more than one service type (e.g. combined PSME and RMM), you have to inform CTS that entities form both services may exist together on the REST. This will prevent CTS from raising errors about unknown RMM types when PSME test package is executed.
        Possible values are: PODM_2_1_2, PSME_2_1_2, RMM_2_1_2, SS_2_1_2, PODM_2_1, PSME_2_1_3, RMM_2_1_3, SS_2_1_3, PODM_2_1_4, PSME_2_1_4, RMM_2_1_4, SS_2_1_4, PODM_2_2, PSME_2_2, RMM_2_2, SS_2_2
        Example:
        `
        ServiceTypeOverride=PSME_2_1,RMM_2_1
        `

    * create hardware_check_list.ini with configuration required by hardware_check_list test script. You may skip this file if checking hardware requirements is not planned. You may also add these keys to config_file.ini created above if you prefer to maintain a single configuration file.
        *  __IsPsmePresent__ - Please declare (Yes/No). Rack must have one or more Pooled System Management Engine software (PSME)
        *  __PowerEfficiency__ - declared power efficiency (number < 100)
        *  __FanPositionNumberingConsistent__ - Please declare (Yes/No). Service personnel should be able to easily identify the location of a failed fan for replacement. It is recommended that the fan position location label use the base as 1 and be numbered from left to right or right to left or top to bottom or bottom to top, within each subsystem (rack, drawer, or module).
        *  __PowerSupplyPositionNumberingConsistent__ - Please declare (Yes/No). Service personnel should be able to easily identify the location of a power supply failure for replacement. It is recommended that the power supply position location label use the base as 1 and be numbered from left to right or top to bottom, within each subsystem (rack, drawer, or module).
        *  __IsRMMNetworkPrivateManagement__ - (Yes/No)
        *  __AreManagementAndProductionNetworksSeparated__ - (Yes/No)
        *  __AreComputeBladesServiceableIndependently__ - (Yes/No)


## Basic Usage
### Browsing available test packages, test suites and test cases
* To list all available test packages:

    `
    cts tests list
    `

* To filter by package name:

    `
    cts tests list -p Rack_Scale_2_2_POD_Manager
    `

* To filter by package and test suite names:

    `
    cts tests list -p Rack_Scale_2_2_POD_Manager -s required
    `

* To generate a sample configuration file for test case:

    `
    cts tests generate_config PACKAGE_NAME TEST_SCRIPT_NAME \
    `

    `
    -o output_file_with_configuration.ini
    `

### Execution

* To simply execute all tests for Rack_Scale_2_2_POD_Manager validation:

    `cts execute tests Rack_Scale_2_2_POD_Manager \ `

    `--config_files config_file.ini`

* To execute only tests for metadata compliance:

    `cts execute tests Rack_Scale_2_2_POD_Manager --test_suites required \ `

    `--config_files config_file.ini`

* To execute only a test validating get responses' compliance with provided metadata:

    `
    cts execute tests Rack_Scale_2_2_POD_Manager --test_scripts validate_get_responses \ `

    `--config_files config_file.ini`

* To set timeout for each script executed, add flag -T --timeout"

    `cts execute tests Rack_Scale_2_2_POD_Manager --config_files config_file.ini \ `

    `-T timeout_in_seconds`

### Test results browsing
* To list all test executions:

    `
    cts status list
    `

* To show detailed information about a specified execution:

    `
    cts status show RUN_ID
    `

* To delete a result from the CTS database:

    `
    cts status delete RUN_ID
    `

* To save results to a file (when you choose *html* option, a new folder called *cts_report* containing *html* files will be created in your working directory):

    `
    cts status dump RUN_ID --output_format [html/csv/text]
    `

### Additional options
* Show CTS Version:

    `
    cts version
    `

* CTS SOS prepare package for easier debugging. Use this command when you suspect CTS crash and want to report issue related with CTS itself. This command collects information such as CTS' logs, network configuration or pip dependencies, etc. After running this command your working directory should contain an `sos-report-<date>` folder and an `sos-report-<date>.tar.gz` archive.

    `
    cts sos
    `


## Advanced Usage

### Using run list to execute multiple tests with a single command

Run list is a mechanism that enables multi-step execution and makes it possible to plan more advanced test scenarios.
Execution of a run list is very similar to executing a test script.


    cts execute run_list run_list_2_2


`run_list_2_1` is a test specification prepared by the user. It defines scope of tests to be executed as well as configuration that should be used to run tests.
Below is an example of a run list that can be used to execute all 2.2 tests:


    $ cat run_list_2_2

    [PSME_2_2]
    TEST_PACKAGE = Rack_Scale_2_2_PSME
    TEST_SUITES = required
    TEST_CONFIGS = ./config/psme.ini, ./config/hardware_check_list.ini

    [StorageServices_2_2]
    TEST_PACKAGE = Rack_Scale_2_2_Storage_Services
    TEST_SUITES = required
    TEST_CONFIGS = ./config/storage.ini

    [PODM_2_2]
    TEST_PACKAGE = Rack_Scale_2_2_POD_Manager
    TEST_SUITES = required
    TEST_CONFIGS = ./config/podm.ini, ./config/hardware_check_list.ini

    [RMM_2_2]
    TEST_PACKAGE = Rack_Scale_2_2_RMM
    TEST_SUITES = required
    TEST_CONFIGS = ./config/rmm.ini, ./config/hardware_check_list.ini

The run list definition refers to additional configuration files:

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

## Test Description

### API Get Validation

You can run the API Get Validation tests by passing a flag:

`
--test_scripts validate_get_responses
`

The test is read-only checking that resources exposed on the REST  service are compliant with metadata being part of the RSD specification. CTS will raise an error if any of the following conditions occur:

* property defined as 'required' is not present
* unknown property is detected in an instance of type that does not allow additional properties
* value of a property has incorrect type
* value of a property has a value out of range (resulting from type itself or when min/max values are defined)
* value of a property does not match a defined regular expression (if any are defined in metadata)
* resource or object of an unknown type is detected


API Get Validation is the most basic test that is available in CTS and as such should be executed as the first test since API correctness is a prerequisite for the rest of the tests.

### API Patch Validation

You can run the API Patch Validation tests by passing a flag:

`
--test_scripts validate_patch_responses
`

The test iterates through all resources discovered on REST API in search of patchable properties (i.e. properties with OData.Permissions/ReadWrite annotation declared in metadata). Based on the property definition, CTS generates a new value and issues a PATCH request followed by GET for verification purposes. If return codes and verification results are correct, the test case passes. Otherwise, CTS reports a warning. When CTS finishes with an actual API resource, the original state is restored.

We usually advise running this test in the next order after the API Get Validation.

### Hardware Checklist Validation

You can run the Hardware Checklist Validation tests by passing a flag:

`
--test_scripts hardware_check_list
`

The test suite consists of both manual and automatic tests that verify that requirements from the Platform Design Specification document are met.

Manual tests require an additional set of configuration parameters to work - please refer to the Test Configuration section. CTS verifies that values provided by the user are conformant with PDS.

The scope of automatic testing is as follows:

* CTS verifies that at least one compute module is present in the POD
* CTS verifies that at least one Ethernet switch is present in the POD
* CTS verifies that at least one Ethernet-based fabric is present in the POD
* CTS verifies that API endpoint uses secure channel (SSL) to serve API
* CTS checks if client is able to perform computer system reset
* CTS checks that all API chassis resources define Location
* CTS checks that chassis hierarchy is consistent
* CTS checks that Power Monitoring is possible at Rack level


### CRUD tests

You can run CRUD (Create Read Update Delete) tests by passing a flag:

`
--test_scripts crud_operations
`

The test tries to create an instance of a resource, then checks if it was created correctly. After that, it attempts to
patch the resource, checks the correctness again and finally deletes it. The test is supposed to clean up the changes it
made no matter when it comes to a stop (e.g. if the service created resources incorrectly, we skip the patch but still try to delete
it). The following resources are tested:

* PSME: VLAN network interface (without patching - not supported by the REST API)
* Storage Services: Logical Drive, Remote Target (for either LVM or CEPH storage)


## Additional functionality

### Metadata diff

CTS can be used not only for testing API compliance, but also for comparing metadata.
The comparison report is saved in a text file.

    $ cts metadata diff --help
    usage: cts diff [-h] [-q [QUALIFIERS [QUALIFIERS ...]]] METADATA METADATA

    positional arguments:
      METADATA              can be any of:
                            - configuration file with remote endpoint definition
                            - RSD release
                              Possible values are:
                              PODM_2_1, PSME_2_1, RMM_2_1, SS_2_1,
                              PODM_2_1_2, PSME_2_1_2, RMM_2_1_2, SS_2_1_2, PODM_2_1_3, PSME_2_1_3, RMM_2_1_3,
                              SS_2_1_3, PODM_2_1_4, PSME_2_1_4, RMM_2_1_4, SS_2_1_4, PODM_2_2, PSME_2_2,
                              RMM_2_2, SS_2_2
                            - path to directory that holds metadata xml files

    optional arguments:
      -h, --help            show this help message and exit
      -q [QUALIFIERS [QUALIFIERS ...]], --qualifiers [QUALIFIERS [QUALIFIERS ...]]



## Troubleshooting
* I ran a test (with the specified *--test_scripts*), but nothing happened.

      `
      $ cts execute tests Rack_Scale_2_2_POD_Manager --test_scripts validate_patch_responses -c config_files.ini

      Using CTS in version 2.2.14.0
      No scripts where selected to execution
      `

  Probably, CTS files in `/opt/cts/` have been created using a root account and their ownership has to be changed:

      `[sudo] chown -R USER_NAME /opt/cts/`

      `[sudo] chmod a+x -R ~/.cts/tests/`

* I got an error `IOError`, when I ran a test.

  Probably, CTS directory `/var/logs/cts/` has been created using a root account and its ownership has to be changed:

    `[sudo] chown -R USER_NAME /var/log/cts`


## Known issues
