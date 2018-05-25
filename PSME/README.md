# Pooled System Management Engine (PSME) Software

PSME Software is a bundle of applications working and communicating with each other to manage and control specific assets in a setup.

# PSME Software

## Agents
- **PSME RMM agent**
    Responsible for management of a racks hardware.
- **PSME Compute agent**
    Responsible for gathering detailed information about compute modules, their telemetry data and for controlling hosts. Participates in Assemble Procedure.
- **PSME Chassis agent**
    Responsible for gathering detailed information about CPP. Communicates with RMM.
- **PSME Network agent**
    Responsible for configuration and gathering detailed information about network topology in the setup on a Drawer level.
- **PSME Storage agent**
    Responsible for preparing, configuring, gathering and connection of storage LVM and tgt. This agent connected with PSME Rest server works as PSME Storage Service.
- **PSME PNC agent**
    Pooled NVMe Controller, responsible for gathering detailed information about PCIe storage switch and attaching NVMe drives to compute hosts.
- **PSME NVMe Target agent**
    Responsible for managing and gathering detailed information about NVMe volumes attached to hosts through RDMA NICs using NVMe over Fabrics (NVMe-oF) technology.
- **PSME NVMe Discovery agent**
    Responsible for responding to queries about available NVMe volumes from NVMe-oF initiators.
- **PSME Compute Simulator agent**
    Reads XML file which describes hardware (assets layout and details), validate it with XML schema and sends them to PSME REST server.
    Imitates PSME Compute agent with data read from XML file. This agent is only used in Deep Discovery feature.

## Services
- **PSME REST Server**
    HTTP server with REST API and JSON data container responsible for gathering and presenting information about assets and available
    operations on these assets. This application speaks with agents through JSON-RPC as a transport and Generic Asset Management
    Interface as a payload protocol. It communicates with appropriate agents.

# Getting started
The PSME Software is designed and developed to support generic hardware. It should compile and run on any Linux* system if the required
libraries are available and at the proper version for the specific operating system. The reference operating systems is Ubuntu 16.04 LTS.

Please refer to the **PSME User Guide** available at [intel.com page](https://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/rack-scale-design-resources.html)
for detailed list of dependencies and compilation instructions.
