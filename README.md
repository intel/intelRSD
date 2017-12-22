# Intel® Rack Scale Design Reference Software

Intel® Rack Scale Design Software is a logical architecture that disaggregates compute, storage, and network resources, and introduces the ability to more efficiently pool and utilize these resources. Intel® Rack Scale Design Software APIs simplify resource management and provide the ability to dynamically compose resources based on workload-specific demands.

More detailed information refer to [official Intel® Rack Scale Design Site](http://intel.com/intelRSD).

## Getting Started

This repository is a good starting point for developers who are ready to start working with Intel® Rack Scale Design Software. For more details on Intel® Rack Scale Design, please visit [Official Intel® Rack Scale Design Project Website](http://intel.com/intelRSD). It is recommended that you read this entire documentation before starting work with your own Rack Scale Design solution. **Keep in mind this code is reference software only.** It is expected that developers will take this reference software and make it their own. 

## Documents

All documents referred to in this ReadMe document can be also found at the [official Intel® Rack Scale Design Site](https://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/rack-scale-design-resources.html).

## Questions and issues
For any questions, inconveniences, found defects in the current Rack Scale Design Reference Software, please create a new issue in this repository. If your questions are about a previous release, include the Rack Scale Design version in your topic.

Please have in mind that adding additional information like including log files, REST API dumps or other OS information while creating a new issue might be very helpful and will shorten a wait time for response.

## News

##### [2017-12-22] New version of Rack Scale Solution Reference Software (2.2) has been released!
The new version of Rack Scale Solution Reference Software (v2.2) has been released! Here is the list of the most important changes:
1.	Out of band Telemetry – Discovering, monitoring and reporting Telemetry metrics,  Health Status of RSD assets through the OOB interface (like IPMI). 
2.	Intel® Xeon® Processor Scalable family (formerly Purley) Platform support – Purley is the next server platform from Intel with lots of improved capabilities around performance, memory architecture and security. The platform is based on cutting-edge technology and provides compelling benefits across a broad variety of usage models including big data, artificial intelligence, high-performance computing, enterprise-class IT, cloud, storage, communication, and the Internet of Things (IoT). 
3.	Digital Signature - For a more secure design, component firmware should be digitally signed and verified at startup. This includes the PSME and PODM components. This is to prevent tampering of the components which could lead to a security attack. 
4.	Arista* TOR switch support - Currently, the PSME Networking agent in RackScale SW v1.2 and v2.1 is designed to manage Red Rock Canyon (RRC) switch. For v2.2, the PSME supports the Arista* DCS-7060CX-32S-R ToR. RSD v2.2 will not support RRC switches. 



## _Disclaimer_

This code is **reference software only and is not feature complete**. 
Intel® makes no claims for the quality or completeness of this code.

*Other names and brands may be claimed as the property of others.
