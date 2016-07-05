# Intel® Rack Scale Design Reference Software

Intel® Rack Scale Design Software is a logical architecture that disaggregates compute, storage, and network resources, and introduces the ability to more efficiently pool and utilize these resources. Intel® Rack Scale Design Software APIs simplify resource management and provide the ability to dynamically compose resources based on workload-specific demands.

More detailed information can be found [here](http://intel.com/intelrackscaledesign).

## _Disclaimer_

This code is a reference software only and is not feature complete. It should not be used in commercial products at this time. Intel® makes no claims for the quality or completeness of this code.


## Getting Started

This *Getting Started* guide is a good starting point for developers who are ready to start working with Intel® Rack Scale Design Software. For details on Intel® Rack Scale Design, please visit [Intel's Open Source Community Website](http://01.org/intelrackscaledesign) or [Official Intel® Rack Scale Design Project Website](http://intel.com/intelrackscaledesign). It is recommended that you read this entire document before starting work with whole solution. Keep in mind this code is reference software only. It is expected that developers will take this reference software and make it their own. Look for more releases in the future! 

Please enjoy!

## Documents
All documents referred in this readme can be also found at the  [official Intel® Rack Scale Design Reference Documents Site](http://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/intel-rack-scale-design-resources.html).

## Using Intel® Rack Scale Design Solution
Follow these steps before using Intel® Rack Scale Design Solution:

1. Before installation, please have an understanding about what hardware you'll be building on, what you have to work with, and how Intel® Rack Scale Design software will work with these components. See [Intel® Rack Scale Design Hardware Platform Design Guide](http://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/hardware-guide.html) for more detailed information.

2. Read fully following documents:
  * [Intel® Rack Scale Design Pooled System Management Engine (PSME) User Guide](http://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/psme-user-guide.html)
  * [Intel® Rack Scale Design Pod Manager (PodM) User Guide](http://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/pod-manager-user-guide.html)
  * [Intel® Rack Scale Design Rack Managment Module (RMM) Software User Guide](http://www.intel.com/content/www/us/en/architecture-and-technology/rack-scale-design/rmm-user-guide.html)
             
       These documents are a great starting point for understanding the core components and how to set up your hardware and software components.

3. Plan out how to configure your Intel® Rack Scale Design software components across your hardware. You'll need to plan out which servers in your rack configuration will run which PSME and PODM components (also known as agents).

    Here is an example configuration (you don't have to do it this way) 
    * Dedicate an Ubuntu 14.04 server with a BMC (e.g 1U on any rack) to run the PSME Compute and Core REST interface modules. For example, Dell PowerEdge, and HP ProLiant have this capability.
    * Dedicate a 10GbE TOR (or other) switch to run the PSME Core and Networking modules.
    * Dedicate another Storage server (disk controller) to run the PSME Core rest APIs and Storage agents/modules. This could potentially be the same dedicated server (with BMC) as referred above since this server could also have a storage controller and additional disks.


4. Download the code you need from our GitHub repository.


5. Read the *PSME Rest API*, *PodM Rest API* and *GAMI API* specifications as a reference and to understand how to work with the code.


6. Build, install and modify the PSME components on your hardware configuration (see *PSME User Guide*). You will need to modify the GAMI agents to interface with your hardware configuration.


7. Build, install, and modify the PodM components to talk with your PSME agents and manage your racks (see *PodM User Guide*).


8. Connect to your outside orchestration layer if you have one (e.g. OpenStack).


9. Read the PSME and PodM *Release Notes* to be aware of issues you may run into.


10. Provide feedback by clicking the *Issues* link in this GitHub repo.



## Pull Requests

Unfortunately, **_we are not upstreaming contributions to the master branch just yet._** There are plans for this in the future. Once the process is established we will post information about it.
