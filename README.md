# Intel<sup>®</sup> Rack Scale Design Reference Software

Intel<sup>®</sup> Rack Scale Design (Intel<sup>®</sup> RSD) reference software is a logical architecture that disaggregates compute,
storage, and network resources while introducing the ability to more efficiently pool and utilize these resources.
Intel<sup>®</sup> RSD software APIs simplify resource management and provide the ability to dynamically compose resources based on workload-specific demands.

## Getting Started

This repository is a good starting point for developers who are interested in working with Intel<sup>®</sup> RSD reference software, and are interesten in building an Intel<sup>®</sup> RSD complaint implementation.
Before you begin work on your own design solution, we recommend reading the available documentation at the official [Intel RSD site](http://intel.com/intelRSD). 

**Note**: This code is reference software only and we expect developers to adapt it to their needs.

## Documents

Any documents referred to in this ReadMe document can be found at the official [Intel<sup>®</sup> RSD site](http://intel.com/intelRSD).

## Support
For questions, concerns, or defects found in the in the current Intel<sup>®</sup> RSD reference software, create a new issue in this repository.
If your questions are about a previous release, include the software version in your topic.

**Note**: When creating a new issue, try to include additional information such as log files,
REST API dumps or other operating system information to expedite response time.


## News

### Coming Soon: Automation through Ansible Galaxy*

Preliminary support for automating the provisioning of RSD hardware via Ansible is available at [Ansible Galaxy](https://galaxy.ansible.com/), enabling users to easily compose and boot an entire datacenter cluster.
It comes in the form of Ansible Role named ‘rsd-provisioning’ and can be found through the [search page](https://galaxy.ansible.com/search), for example, by using the “RSD” keyword, or by looking up
"Intel contributions" in the advanced search form.
This Ansible Role comes with an initial set of modules for the following operations: composition and deletion, booting and power option controls. 
More information on their usage and full set of available options can be found in the associated README file.

At the time of writing the aforementioned modules are being proposed to the community, to be made available through the standard Ansible distribution under the “Remote Management” category. 
These modules are currently pending review and approval, the final location and acceptance is subject to change without notice.


### [July 2019] v2.5 of the Intel<sup>®</sup> RSD reference software has been released!

This release is based on the Redfish<sup>®</sup> specification [v1.6.1](https://www.dmtf.org/sites/default/files/standards/documents/DSP0266_1.6.1.pdf).
It uses the Redfish<sup>®</sup> Schema [v2018.3](https://www.dmtf.org/sites/default/files/standards/documents/DSP8010_2018.3.zip)
and Swordfish<sup>®</sup> Schema [v1.0.7a](https://www.snia.org/sites/default/files/technical_work/Swordfish/Swordfish_v1.0.7a.zip).

The interfaces for various resources are enhanced with Intel<sup>®</sup> RSD extensions.

Below is a list of the most important features added in this software version:
*	Compute Log Service - records from the System Events Log on a platform are now presented in a Redfish LogService.
*	CTS includes support for Redfish<sup>®</sup> metadata 2019.1 (with Swordfish 1.0.7, included in Redfish Metadata 2018.3 & 2019.1) in regular tests and support for Rack Scale & OCP<sup>®</sup> profiles.


### [April 2019] v2.4 of the Intel<sup>®</sup> RSD reference software has been released!

This release is based on the Redfish<sup>®</sup> specification [v1.5.0](https://www.dmtf.org/sites/default/files/standards/documents/DSP0266_1.5.0.pdf).
It uses the Redfish<sup>®</sup> Schema [v2018.1](https://www.dmtf.org/sites/default/files/standards/documents/DSP8010_2018.1.zip)
and Swordfish<sup>®</sup> Schema [v1.0.6](https://www.snia.org/sites/default/files/technical_work/Swordfish/Swordfish_v1.0.6.zip).
The exceptions are as follows:

*   the interface for Manager and Memory resources is based on Redfish<sup>®</sup> Schema [v2018.2](https://www.dmtf.org/sites/default/files/standards/documents/DSP8010_2018.2.zip).

*	the interface for FPGA is based on a Redfish<sup>®</sup> WIP [IS0007](https://www.dmtf.org/sites/default/files/standards/documents/DSP-IS0007_0.9a.zip).

*	the interface for Telemetry is based on a Redfish<sup>®</sup> WIP [DSP2051](https://www.dmtf.org/sites/default/files/standards/documents/DSP2051_0.1.0a.zip).

*	the interfaces for various resources are enhanced with Intel<sup>®</sup> Rack Scale Design extensions.

Below is a list of the most important features added in this software version:

*	**FPGA Pooling over Fabrics**

    Enables composition of nodes connected to FPGAs via Remote Direct Memory Access (RDMA) over
	Converged Ethernet protocol through the Top of Rack switches (TORs) or leaf switches.

*	**FPGA Pooling over PCIe**

    Enables composition of nodes connected to FPGAs via a PCIe Switch.

*   **Optane DC Persistent Memory management**

    Enables discovery of Optane DC Persistent Memory modules and composition of nodes containing those modules.

*   **Redfish authentication**

    The RSD services now conform to Redfish* requirements for authentication.

*   **High Availability** 
    
    PODM High Availability is provided by the Kubernetes* platform, which allows for running multiple instances of PODM components. In case of failure of some instance, other nodes of cluster will take over and Kubernetes will bring back online the missing instance - without disrupting PODM functions.

### [September 2018] v2.3.2 of the Intel<sup>®</sup> RSD reference software has been released!

Important change to the new software version:

*	**Added the NVMe Initiator Wheel script (`nvme-wheel`)**

    An initiator host polls the Discovery Service to ensure that its connections to remote volumes are up-to-date.
	And the NVMe Intiator Wheel script, `nvme-wheel`, is a tool that performs these actions.

    NVMe-Wheel is located [here](https://github.com/intel/intelRSD/tree/master/PSME/nvme-wheel/) 

### [July 2018] v2.3.1 of the Intel<sup>®</sup> RSD reference software has been released!

Important change to the new software version:

*	**Updated the Conformance Test Suite (CTS)**

    A new set of CRUD tests designed to verify NVMe\* over Fabrics (NVMe-oF) operations has been added.

### [May 2018] v2.3 of the Intel<sup>®</sup> RSD reference software has been released!

Below is a list of the most important changes to the new software version:

1.	**NVMe\* over Fabrics (NVMe-oF)**

    Enables composition of nodes connected to NVM Express (NVMe)\* drives via Remote Direct Memory Access (RDMA) over
	Converged Ethernet protocol through the Top of Rack switches (TORs) or leaf switches.

2.	**Quality of Service (QoS)**

    Supports setting some QoS parameters to configure the leaf switches. It is required to allocate the capability
	of a network for selected network traffic over Ethernet. NVMe-oF is one of the traffic types that requires an
	appropriate level of network resources to be allocated.

3.	**Standards-Based Storage Management**

    The Intel<sup>®</sup> RSD Storage Services REST API is aligned to the Storage Networking Industry Association (SNIA) Swordfish\* specification.

## Disclaimer

This code is reference software only and is not feature complete. Intel<sup>®</sup> makes no claims for the quality or completeness of this code.

Intel, the Intel logo, are trademarks of Intel Corporation or its subsidiaries in the U.S. and/or other countries.

Copies of documents that have an order number and are referenced in this document may be obtained by calling 1 800 548 4725 or by visiting www.intel.com/design/literature.htm.

*Other names and brands may be claimed as the property of others.  (when using third-party trademarks and names) 

Copyright © 2019 Intel Corporation. All rights reserved.
