# Pooled System Management Engine (PSME) Software

PSME Software is a bundle of applications working and communicating with each other to manage and control specific assets in a setup.

# PSME Software

## Agents
- **PSME Rack Management Module (RMM) Agent**:
    is responsible for management of a rack's hardware.
- **PSME Compute agent**:
    gathers detailed information about the Compute modules, their telemetry data, and for controlling hosts - participates in Assemble Procedure.
- **PSME Chassis agent**:
    gathers detailed information about the CPP. The PSME Chassis agent communicates with RMM.
- **PSME Network agent**:
    configures and gathers detailed information about the network topology on the Top of Rack (TOR) switch.
- **PSME Internet Small Computer System Interface (iSCSI) Storage agent**:
    prepares and configures **LVM** Logical Volumes and manging the target (tgt) iSCSI daemon.
- **PSME Pooled Node Controller (PNC) agent**:
    gathers detailed information about the Peripheral Component Interconnect express* (PCIe*) storage and accelerator switch and
    attaching non-volatile memory express* (NVMe*) drive and field-programmable gate arrays (FPGAs) to compute hosts.
- **PSME NVMe GUID Partition Table (GPT) Target agent**:
    manages the GPT partitions attached to remote hosts through remote direct memory access (RDMA) network interface cards (NICs)
    using the **nvmet kernel** module implementation of NVMe over Fabrics* (NVMe-oF*).
- **PSME NVMe SPDK Target agent**:
    manages the Storage Performance Development Kit implementation of NVMe-oF.
- **PSME NVMe Discovery agent**:
    responds to queries about available NVMe volumes from NVMe-oF initiators.
- **PSME FPGA-oF Discovery agent**:
    responds to queries about target services with available FPGAs from FPGA-oF initiators.
- **PSME FPGA over Fabric (FPGA-oF) Target agent**:
    manages and gathers detailed information about FPGAs attached to hosts through RDMA NICs using FPGA-oF protocol.
- **PSME FPGA Discovery agent**:
    responds to queries about available FPGAs from FPGA-oF initiators.


## Services
 **PSME REST Server**:

    The HTTP server with REST API and JSON data container is responsible for gathering and presenting information about assets and available
    operations on these assets. This application communicates with the appropriate agents through JSON-RPC as a transport and the Generic Asset Management
    Interface as a payload protocol.

# Getting started
The PSME Software is designed and developed to support generic hardware. It should compile and run on any Linux* system if the required
libraries are available and at the proper version for the specific operating system. The reference operating systems is Ubuntu* v16.04 LTS.
The PSME Network agent works on Arista* EOS.

Refer to the **PSME User Guide** available at [intel.com page](https://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/rack-scale-design-resources.html)
for detailed list of dependencies and compilation instructions.
