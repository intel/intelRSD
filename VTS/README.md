# Validation Test Suite

## Installation

### Install prerequisites
 * On Ubuntu systems:

     ```
     apt-get -y update
     apt-get -y install python-pip python-setuptools git python-lxml python-dev
     ```

 * On Redhat based systems:

     ```
     dnf update -y
     dnf install -y python-pip python-setuptools git python-lxml python-devel redhat-rpm-config
     ```

### Installation from source code
1. Remove old /opt/vts data
	```
	[sudo] rm -rf /opt/vts
	```

2. Download source code
3. Enter directory:

	```
	cd VTS/sources
	```

4. Create VTS egg:

	```
	python setup.py bdist_egg
	```
5. Install VTS egg by typing (easy_install requires correctly configured connection to public repositories (proxy, routes, etc)):

	```
	[sudo] easy_install dist/*.egg
	```
6. Copy the configuration.ini file to location /etc/vts/configuration.ini:

	```
	[sudo] mkdir -p /etc/vts/
	[sudo] cp configuration.ini /etc/vts/
	```
7. Copy test scripts to /opt/vts/tests:

    ```
    [sudo] mkdir -p /opt/vts/tests/
    [sudo] cp -r tests_packages/* /opt/vts/tests/
    ```

8. Create directory for logs:

    ```
    [sudo] mkdir -p /var/log/vts
    ```

9. If VTS files have been created using a root account, their ownership has to be changed to the user that will be executing VTS:

    ```
    [sudo] chown -R USER_NAME /opt/vts/
    [sudo] chown -R USER_NAME /var/log/vts
    ```


### Installation from pre-built .tar.gz archive
1. Remove old /opt/vts data
	```
	[sudo] rm -rf /opt/vts
	```

2. Download tar.gz package with VTS

3. Extract tar.gz package:

    ```
    tar -zxvf VTS_PACKAGE.tar.gz
    ```
4. Enter extracted directory:

    ```
    cd VTS_PACKAGE
    ```
5. Install vts egg (easy_install requires correctly configured connection to public repositories: proxy, routes, etc)

    ```
    [sudo] easy_install Vts*.egg
    ```
6. Copy the configuration.ini file to location /etc/vts/configuration.ini:

	```
	[sudo] mkdir -p /etc/vts/
	[sudo] cp configuration.ini /etc/vts/
	```
7. Copy test scripts to /opt/vts/tests:

    ```
    [sudo] mkdir -p /opt/vts/tests/
    [sudo] cp -r tests_packages/* /opt/vts/tests/
    ```

8. Create directory for logs:

    ```
    [sudo] mkdir -p /var/log/vts
    ```

9. If VTS files have been created using a root account, their ownership has to be changed to the user that will be executing VTS:

    ```
    [sudo] chown -R USER_NAME /opt/vts/
    [sudo] chown -R USER_NAME /var/log/vts
    ```


## Configuration
* VTS framework configuration
    *  Open file /etc/vts/configuration.ini
    * Following flags can be edited:
        * __SQL_CONNECTION_STRING__ - defines database to be used by vts

            set flag following instructions
            http://docs.sqlalchemy.org/en/latest/core/engines.html
            (can require installing additional packages)
* DMTF metadata tests configuration:
    *  create config_file.ini with flags:
        *  __ApiEndpoint__ - endpoint to API in format ip:port
        *  __MetadataLocationDirectory__ - location of metadata that shall be used to validate API behaviour
        *  __UseSSL__ - defines if VTS shall use http or https protocol to connect to api (Yes/No)
        *  __CertificateCertFile__ and __CertificateKeyFile__ - paths to client side pem certificate and key files (if API requires client certificate authorization)
        *  __User__ and __Password__ - User and Password used by VTS to authorize (if API requires basic authorization)

## Usage
### Browsing available test packages, test suites and test cases
* To list all available test packages:

    ```
    vts tests list
    ```
* To filter by package name:

    ```
    vts tests list -p RackScale_1_2
    ```
* To filter by package and test suite names:

    ```
    vts tests list -p RackScale_1_2 -s dmtf
    ```
* To filter by package,test suite and test case names:

    ```
    vts tests list -p RackScale_1_2 -s dmtf -t validate_get_responses
    ```
* To generate sample configuration file for test case:

    ```
    vts tests generate_config PACKAGE_NAME TEST_SCRIPT_NAME \
    -o output_file_with_configuration.ini
    ```

### Execution
* To simply execute all tests for RackScale_1_2 validation:

    ```
    vts execute tests --package RackScale_1_2 --config_files config_file.ini
    ```
* To execute only tests for metadata compliance:

    ```
    vts execute tests --package RackScale_1_2 --test_suites dmtf \
    --config_files config_file.ini
    ```
* To execute only test validating get responses compliance with provided metadata:

    ```
    vts execute tests --package RackScale_1_2 --test_scripts validate_get_responses \
    --config_files config_file.ini
    ```
* To set timeout for each script executed add flag -T --timeout"

    ```
    vts execute tests --package RackScale_1_2 --config_files config_file.ini \
    -T timeout_in_seconds
    ```

### Tests results browsing
* To list all tests executions:

    ```
    vts status list
    ```
* To show detailed information about specified execution:

    ```
    vts status show RUN_ID
    ```
* To delete result from VTS database:

    ```
    vts status delete RUN_ID
    ```
* To save results to file:

    ```
    vts status dump RUN_ID --output_format [html/csv/text]
    ```

### Additional options
* Show VTS Version:

    ```
    vts version
    ```
