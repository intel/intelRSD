# Intel® Rack Scale Design Reference Software

Intel® Rack Scale Design Software is a logical architecture that disaggregates compute, storage, and network resources, and introduces the ability to more efficiently pool and utilize these resources. Intel® Rack Scale Design Software APIs simplify resource management and provide the ability to dynamically compose resources based on workload-specific demands.

More detailed information can be found at [official Intel® Rack Scale Design Site](http://intel.com/intelRSD).

## Getting Started

This repository is a good starting point for developers who are ready to start working with Intel® Rack Scale Design Software. For more details on Intel® Rack Scale Design, please visit [Official Intel® Rack Scale Design Project Website](http://intel.com/intelRSD). It is recommended that you read this entire documentation before starting work with your own Rack Scale Design solution. **Keep in mind this code is reference software only.** It is expected that developers will take this reference software and make it their own. 

## Documents

All documents referred to in this readme can be also found at the [official Intel® Rack Scale Design Site](http://intel.com/intelRSD).

## Questions & issues
For any questions, inconveniences, found defects in the current Rack Scale Design Reference Software, please create a new issue in this repository. If your questions are about a previous release, please include the Rack Scale Design version in your topic.

Please have in mind that adding additional information like including log files, REST API dumps or other OS information while creating a new issue might be very helpful and will shorten a wait time for response.

## News

##### [2017-06-02] New version of Rack Scale Solution Reference Software (2.1.3) has been released!
The new version of Rack Scale Solution Reference Software (2.1.3) has been released! Here is the list of the most important changes:
* Added new component *Simulators*
* Added SSDP support for PODM/PSME/RSS services
* Updated REST API to current Redfish Specification
* PODM has changed its DB to PostgreSQL
* Added tree stability feature
* Added support for PNC switch
* Added support for iSCSI Out of Band booting
* Added CHAP authentication for iSCSI Targets
* Added possibility to set LocationID as string
* Added CRUD operations on VLANs
* PODM is now able to read data from current and previous release


## _Disclaimer_

This code is **reference software only and is not feature complete**. Intel® makes no claims for the quality or completeness of this code.
