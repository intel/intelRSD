# Pooled System Management Engine (PSME) Software

PSME Software is a bundle of applications working and communicating with each other to manage and control specific assets in a setup.

## Agents
PSME Software consists of:
- **PSME REST Server** - HTTP server with REST API and JSON data container responsible for gathering and presenting information about assets and available operations on these assets. This application speaks with agents through JSON-RPC as a transport and Generic Asset Management Interface as a payload protocol. It communicates with agents:
    - **PSME Compute agent**
    - **PSME Compute agent simulator**
    - **PSME Network agent**
    - **PSME Chassis agent**
    - **PSME Storage agent**
    - **PSME PNC agent**
- **PSME Compute Agent** - responsible for gathering detailed information about compute modules and for controlling hosts. Participates in Assemble Procedure.
- **PSME Compute Agent Simulator** - reads XML file which describes hardware (assets layout and details), validate it with XML schema and sends them to PSME REST server. Imitates PSME Compute agent with data read from XML file. This agent is only used in Deep Discovery feature.
- **PSME Network Agent** - responsible for configuration and gathering detailed information about network topology in the setup on a Drawer level.
- **PSME Chassis Agent** - responsible for gathering detailed information about CPP. Communicates with RMM.
- **PSME Storage Agent** - responsible for preparing, configuring, gathering and connection of storage LVM and tgt. This agent connected with PSME Rest server works as PSME Storage Service.
- **PSME Pooled NVMe Controller (PNC) agent** - responsible for gathering detailed information about PCIe storage switch and attaching NVMe drives to compute hosts.

## Getting started
The PSME Software is designed and developed to support generic hardware. It should compile and run on any Linux* system if the required libraries are available and at the proper version for the specific operating system. The reference operating systems were Fedora 23 and Ubuntu 16.04 LTS.

Please refer to the **PSME User Guide** available at [01.org page](https://01.org/group/4562/documentation-list) for detailed list of dependencies and compilation instructions.