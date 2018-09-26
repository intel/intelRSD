# Intel® Rack Scale Design Reference Software

Intel® Rack Scale Design (Intel® RSD) reference software is a logical architecture that disaggregates compute, storage, and network resources while introducing the ability to more efficiently pool and utilize these resources. Intel® RSD software APIs simplify resource management and provide the ability to dynamically compose resources based on workload-specific demands.

## Getting Started

This repository is a good starting point for developers who are interested in working with Intel RSD reference software. Before you begin work on your own design solution, we recommend reading the available documentation at the official [Intel RSD site](http://intel.com/intelRSD). 

**Note**: This code is reference software only and we expect developers to make it their own. 

## Documents

Any documents referred to in this ReadMe document can be found at the official [Intel RSD site](http://intel.com/intelRSD).

## Support
For questions, concerns, or defects found in the in the current Intel RSD reference software, please create a new issue in this repository. If your questions are about a previous release, include the software version in your topic.

**Note**: When creating a new issue, try to include additional information such as log files, REST API dumps or other operating system information to expedite response time.

## News

### [September 2018] Version 2.3.2 of the Intel RSD reference software has been released!

Important change to the new software version:

*	**Added the NVMe Initiator Wheel script (`nvme-wheel`)**

    An initiator host polls the Discovery Service to ensure that its connections to remote volumes are up-to-date. And the NVMe Intiator Wheel script, `nvme-wheel`, is a tool that performs these actions.

    [NVMe-Wheel is located under PSME directory](https://github.com/intel/intelRSD/PSME/nvme-wheel/dist) 

### [July 2018] Version 2.3.1 of the Intel RSD reference software has been released!

Important change to the new software version:

*	**Updated the Conformance Test Suite (CTS)**

    A new set of CRUD tests designed to verify NVMe\* over Fabrics (NVMe-oF) operations has been added.

### [May 2018] Version 2.3 of the Intel RSD reference software has been released!

Below is a list of the most important changes to the new software version:

1.	**NVMe\* over Fabrics (NVMe-oF)**

    Enables composition of nodes connected to NVM Express (NVMe)\* drives via Remote Direct Memory Access (RDMA) over Converged Ethernet protocol through the Top of Rack switches (TORs) or leaf switches.

2.	**Quality of Service (QoS)**

    Supports setting some QoS parameters to configure the leaf switches. It is required to allocate the capability of a network for selected network traffic over Ethernet. NVMe-oF is one of the traffic types that requires an appropriate level of network resources to be allocated.

3.	**Standards-Based Storage Management**

    The Intel RSD Storage Services REST API is aligned to the Storage Networking Industry Association (SNIA) Swordfish\* specification.

## Disclaimer

This code is reference software only and is not feature complete. Intel makes no claims for the quality or completeness of this code.

\*Other names and brands may be claimed as the property of others.
