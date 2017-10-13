# POD Manager

The POD Manager is a Java-based application which provides the REST-based interface that allows full management of Intel® Rack Scale Design solution including asset discovery, configuration and nodes assembly.

## Getting Started
This chapter contains shortened information about building POD Manager solution by user. To get more information please check the full version of **POD Manager User Guide** available at the [official Intel® Rack Scale Design Site](http://intel.com/intelRSD).

### Prerequisites
#### OS
POD Manager software can be installed on both Ubuntu Server 14.04.4 or Ubuntu Server 16.04.1 **(preferred)**.

#### Database
POD Manager requires [PostgreSQL](https://www.postgresql.org/) database preinstalled on the OS. Currently supported version is **9.5**

#### Other dependencies
(*Following dependencies are not required to build POD Manager Software or development process however they are required to be installed on real HW*)

User also needs to install following Debian packages:
* isc-dhcp-server
* openssh-server
* python3 (already preinstalled on Ubuntu 16.04.1)
* tftpd-hpa
* ntp
* vlan
* acl

### Building software
POD Manager is written in Java programming language. User needs to install **OpenJDK 8** to be able to build this software.

The easiest way to get POD Manager built and install is to create *.deb packages by executing following command in `SW` directory:
```
$ ./gradlew buildDeb
```
Or alternatively if you have Gradle installed in your path
```
$ gradle buildDeb
```

and then install `pod-manager_[version]_all.deb` file created under following path:

```
[projectRoot]/pod-manager/build/distributions/pod-manager_[version]_all.deb
```
