# Validation Test Suite

## Installation
1. Download source code
2. Enter directory:

	```
	cd VTS/sources
	```
3. Install all system dependencies:
     * On Ubuntu by typing:
     ```
     apt-get -y update
     apt-get -y install python-pip python-setuptools git python-lxml python-dev
     ```
     * On Redhat based systems by typing:
     ```
     dnf update -y
     dnf install -y python-pip python-setuptools git python-lxml python-dev
     ```
4. Create VTS egg:

	```
	python setup.py bdist_egg
	```
5. Install VTS egg by typing (easy_install requires correctly configured connection to public repositories (proxy, routes, etc)):

	 ```
	easy_install vts.egg
	```
6. Copy the configuration.ini file to location /etc/vts/configuration.ini:

	```
	mkdir -p /etc/vts/
	cp configuration.ini /etc/vts/
	```
7. Copy test scripts to /opt/vts/tests:

    ```
    mkdir -p /opt/vts/tests/
    cp -r tests_packages/* /opt/vts/tests/
    ```
8. If VTS files have been created using a root account, their ownership has to be changed to the user that will be executing VTS:

    ```
    chown -R USER_NAME /opt/vts/
    chown -R USER_NAME /var/log/vts
    ```
    
## Configuration
* VTS framework configuration:
    * Open file /etc/vts/configuration.ini
    * Following flags can be edited:
        * __SQL_CONNECTION_STRING__ - defines database to be used by VTS - set flag following instructions http://docs.sqlalchemy.org/en/latest/core/engines.html (can require installing additional packages)
        * __REPORTS_DIRECTORY__ - defines directory for VTS to keep logs from VTS runs - VTS requires r/w permission to store logs.
* DMTF metadata tests configuration:
    *  create config_file.ini with flags:
        *  __ApiEndpoint__ - endpoint to API in format ip:port
        *  __MetadataLocationDirectory__ - location of metadata that shall be used to validate API behaviour
        *  __UseSSL__ - defines if VTS shall use http or https protocol to connect to api (Yes/No)
        *  __CertificateCertFile__ and __CertificateKeyFile__ - paths to client side pem certificate and key files (if API requires client certificate authorization)
        *  __User__ and __Password__ - User and Password used by VTS to authorize (if API requires basic authorization)
## Usage
### Browsing available test packages, test suites and test cases
* To list all available test packages type:
```
vts tests list
```
* To show all available test suites add flag -s type:
```
vts tests list -s
```
* To show all available test cases add flag -t type:
```
vts tests list -t
```
* To filter available test suites by package name type:
```
vts tests list -p RSA_1_2 -s
```
* To filter available test cases by package and test suite name type:
```
vts tests list -p RSA_1_2 -s dmtf -t
```
### Execution
* To simply execute all tests for RackScale_1_2 validation type:
```
vts execute tests --package RackScale_1_2 --config_files config_file.ini
```
* To execute only tests for metadata compliance type:
```
vts execute tests --package RackScale_1_2 --test_suites dmtf --config_files config_file.ini
```
* To execute only test validating get responses compliance with provided metadata type:
```
vts execute tests --package RackScale_1_2 --test_cases ValidateGetResponses --config_files config_file.ini
```
### Tests results browsing
* To list all tests executions type:
```
vts status list
```
* To show detailed information about specified execution type:
```
vts status show RUN_ID
```
* To delete result from VTS database type:
```
vts status delete RUN_ID
```
### Additional options
* Show VTS version:
```
vts version
```

## Testing limitations

### HTTP PATCH request testing
* VTS is testing PATCH request only on properties marked explicitly as READ/WRITE
* VTS is not testing PATCH request on properties that are not marked as READ/WRITE or READONLY

### Warnings for current release
VTS testing is based on metadata. Due to metadata errors VTS is not testing HTTP PATCH request on the following properties:
   * On Ethernet Switch Ports:
      * AdministrativeState
      * LinkSpeedMbps
      * FrameSize
      * Autosense
      * PrimaryVlan
   * On Ethernet Switch ACL Rule:
      * RuleId
      * Action
      * ForwardMirrorInterface
      * MirrorPortRegion
      * MirrorType
      * Condition
 

