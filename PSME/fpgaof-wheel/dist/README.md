# FPGA over Fabric Initiator Wheel

## Table of Contents
* [Overview](#overview)
* [Installation of the FPGAoF Initiator Wheel](#installation-of-the-fpgaof-initiator-wheel)
    * [Dependencies](#dependencies)
* [Running `fpgaof-wheel`](#Running-fpgaof-wheel)
    * [Run `fpgaof-wheel` with Example Parameters](#run-fpgaof-wheel-with-example-parameters)
    * [Run `fpgaof-wheel` in the Background](#run-fpgaof-wheel-in-the-background)
    * [Run `fpgaof-wheel` in cron table](#run-fpgaof-wheel-in-cron-table)
    * [Get the UUID](#get-the-uuid)
    * [Change the Default Discovery Time](#change-the-default-discovery-time)


**Note**: The FPGAoF Initiator Wheel script, `fpgaof-wheel`, was developed and tested on Ubuntu\* version 18.04.1 with Python\* version 2.7.14.

## Overview
An initiator host polls the Discovery Service to ensure that its connections to remote FPGAs are up-to-date. And the FPGAoF Initiator Wheel script, `fpgaof-wheel`, is a tool that performs these actions.

This ReadMe describes the operation of the FPGAoF Initiator Wheel, which is installed on an initiator host.

Discovered targets are saved in /etc/opae/discovery_service.cfg file.

### Dependencies
Installation of `fgpaof-wheel` requires a wheel module linked with Python2\*.

**Note**: Wheel is a packaging format for Python and meant to replace the Egg format. To learn more about Wheel, go to https://pythonwheels.com/.

1. To install Python\* and Pip Installs Packages (PIP):
```bash
[sudo] apt-get install python-pip python-dev build-essential
```

[Alternative] If the previous step does not set a PIP, use an easy install:
```bash
sudo easy_install pip
```

2. To install the wheel module:
```bash
pip install wheel
```

3. To install additional requirements:
```bash
pip install -r requirements.txt
```

3. To install the `fpgaof-wheel`:
```bash
[sudo] bash INSTALL.sh
```

or

```bash
[sudo] -H pip install FPGAoF_Wheel*
```

**Note**: "Wheel" is a module that allows users to install .whl packages. And `fpgaof-wheel` is the name of the script that we run in the terminal window.

## Running `fpgaof-wheel`
To show available commands, type:

```bash
fpgaof-wheel
```

To run in continuous mode, use the `execute` command:

```bash
fpgaof-wheel execute
```

For more information about custom arguments, run:

```bash
fpgaof-wheel execute --help
```

### Run `fpgaof-wheel` with Example Parameters
Below is an example of running the `fpgaof-wheel` command with example parameters:

```bash
fpgaof-wheel execute -a 10.6.0.101 -s 4420
```

If you use an custom UUID, which is different than your system Universally Unique Identifier (UUID), type:
```bash
fpgaof-wheel execute -a 10.6.0.101 -s 4420 -q custom_uuid
```

### Change the Default Discovery Time
The `fpgaof-wheel` sends a post to Discovery Service every 10 seconds. This default value can be changed with -t option. For example:

```bash
fpgaof-wheel execute -a 10.6.0.101 -s 4420 -t 15
```


### Change the maximum retries (in succession) to connect with Discovery Service
The `fpgaof-wheel` can limit unsucessful attempts to connect with Discovery Service. This value can be enabled and changed with -e option. For example:

```bash
fpgaof-wheel execute -a 10.6.0.101 -s 4420 -e 2
```


### Run `fpgaof-wheel` in the Background
To run in continuous mode and in the background, you can use `nohup`:

```bash
nohup fpgaof-wheel execute <<additional_args>> &
```

If `nohup` isn't available on your machine, try:

```bash
fpgaof-wheel execute <<additional_args>> &
```

### Run `fpgaof-wheel` in cron table
Run the script automatically after a reboot by adding this line below, into cron table (crontab):

In terminal type `crontab -e` to open a crontab and type:

```bash
@reboot sudo -H fpgaof-wheel execute <<additional_args>>
```

### Get the UUID
To print the UUID, use:

```bash
fpgaof-wheel uuid
```
