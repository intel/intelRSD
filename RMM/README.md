# Rack Management Module (RMM)
RMM is a software module that enables management of the racks hardware.

## Building
This chapter contains shortened information about building RMM solution by user. To get more information please check the full version of **RMM User Guide** available at the [official Intel® Rack Scale Design Site](http://intel.com/intelRSD).

### Prerequisites
The minimum version of Ubuntu needed for compiling RMM software is **12.04**.

Following dependencies need to be installed on the OS:
- build-essenttal
- cmake 2.8.12
- automake 1.4.1
- autoconf 2.69

### x86:
```
$ cd utils
$ ./rmm_release.sh
```

### ARM:
```
cd utils
$ ./rmm_release.sh arm
```

All output files are generated under `build` directory.


## Installing

### x86
After running `rmm_release.sh`, a few deb files are generated and packed into a tarball, For example:
```
build/release/rsa-rmm-[version].tar.gz
```

You can extract deb files from it and install them to your RMM linux system by running command like:
```
$ cd rsa-rmm-[version]
$ sudo dpkg -i *.deb
```


### ARM
User need to copy needed executable binaries and libraries to your target system from below directories:
```
build/bin
build/lib
```


## Starting/Stopping RMM service
### x86
```
$ sudo service rmm start
```
or
```
$ sudo service rmm stop
```

### ARM
User need to write a script to run all needed executable files specific for its usage.

## Openssl declaration
This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)
This product includes cryptographic software written by Eric Young (eay@cryptsoft.com).  
This product includes software written by Tim Hudson (tjh@cryptsoft.com).

