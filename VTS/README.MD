# Installation
1. Download tar.gz package with VTS
2. Extract tar.gz package:
```
tar -zxvf VTS_PACKAGE.tar.gz
```
3. Enter extracted directory:
```
cd VTS_PACKAGE
```
4. Install all system dependencies:
     1. On ubuntu by typing:
     ```
     apt-get -y update
     apt-get -y install python-pip python-setuptools git python-lxml python-dev
     ```
     2. On Redhat based systems by typing:
     ```
     dnf update -y
     dnf install -y python-pip python-setuptools git python-lxml python-dev
     ```
5. Install cts egg by typing (easy_install requires correctly configured connection to public repositories (proxy, routes, etc)):
```
easy_install vts.egg
```
6. Copy the configuration.ini file to location /etc/vts/configuration.ini
```
mkdir -p /etc/vts/
cp configuration.ini /etc/vts/
```
7. Copy test scripts to /opt/vts/tests
```
mkdir -p /opt/vts/tests/
cp -r tests_packages/* /opt/vts/tests/
```
8. If vts files have been created using a root account, their ownership has to be changed to the user that will be executing vts:
```
chown -R USER_NAME /opt/vts/
chown -R USER_NAME /var/log/vts
```
# Configuration
* VTS framework configuration
    * Open file /etc/vts/configuration.ini
    * Following flags can be edited:
        * __SQL_CONNECTION_STRING__ - defines database to be used by cts - set flag following instructions http://docs.sqlalchemy.org/en/latest/core/engines.html (can require installing additional packages)
        * __REPORTS_DIRECTORY__ - defines directory for cts to keep logs from cts runs - cts requires r/w permission to store logs.
* DMTF metadata tests configuration:
    *  create config_file.ini with flags:
        *  __ApiEndpoint__ - endpoint to API in format ip:port
        *  __MetadataLocationDirectory__ - location of metadata that shall be used to validate API behaviour
        *  __UseSSL__ - defines if CTS shall use http or https protocol to connect to api (Yes/No)
        *  __CertificateCertFile__ and __CertificateKeyFile__ - paths to client side pem certificate and key files (if API requires client certificate authorization)
        *  __User__ and __Password__ - User and Password used by CTS to authorize (if API requires basic authorization)
# Usage
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
* Show VTS Version:
```
vts version
```

# Release Notes
* Due to metadata issue VTS may verify too restrictive derivated complex type when expected complex type is simpler than actual without explicitly defined odata.type mark. It's known issue with Links property on Manager.1.0.2.Manager resource. 