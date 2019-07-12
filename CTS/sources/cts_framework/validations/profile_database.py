OCP_PROFILE_1_0_0 = """
{
  "SchemaDefinition": "RedfishInteroperabilityProfile.v1_0_0",
  "ProfileName": "OCPBaselineHardwareManagement",
  "ProfileVersion": "1.0.0",
  "Purpose": "Specifies the OCP baseline hardware management requirements for the Redfish interface on platforms.",
  "OwningEntity": "Open Compute Project",
  "ContactInfo": "hwpl@opencompute.org",
  "Protocol": {
    "MinVersion": "1.0",
    "Discovery": "Recommended",
    "HostInterface": "Recommended"
  },
  "Resources": {
    "AccountService": {
      "PropertyRequirements": {
        "Accounts": {}
      }
    },
    "Chassis": {
      "PropertyRequirements": {
        "AssetTag": {
          "ReadRequirement": "Recommended",
          "WriteRequirement": "Recommended"
        },
        "ChassisType": {},
        "IndicatorLED": {
          "ReadRequirement": "Recommended",
          "WriteRequirement": "Recommended"
        },
        "Manufacturer": {},
        "Model": {},
        "SerialNumber": {},
        "SKU": {
          "ReadRequirement": "Recommended"
        },
        "PartNumber": {
          "ReadRequirement": "Recommended"
        },
        "PowerState": {},
        "Status": {
          "PropertyRequirements": {
            "State": {},
            "Health": {}
          }
        },
        "Thermal": {
          "ReadRequirement": "Recommended"
        },
        "Power": {
          "ReadRequirement": "Recommended"
        },
        "Links": {
          "PropertyRequirements": {
            "ManagedBy": {}
          }
        }
      }
    },
    "ChassisCollection": {
      "PropertyRequirements": {
        "Members": {
          "MinCount": 1
        }
      }
    },
    "EthernetInterface": {
      "MinVersion": "1.1.0",
      "ReadRequirement": "Recommended",
      "ConditionalRequirements": [
        {
          "SubordinateToResource": [
            "Manager",
            "EthernetInterfaceCollection"
          ],
          "ReadRequirement": "Mandatory"
        }
      ],
      "PropertyRequirements": {
        "MACAddress": {},
        "SpeedMbps": {},
        "InterfaceEnabled": {},
        "LinkStatus": {},
        "Status": {
          "PropertyRequirements": {
            "Health": {},
            "State": {}
          }
        },
        "DHCPv4": {
          "ReadRequirement": "Recommended",
          "WriteRequirement": "Recommended"
        },
        "DHCPv6": {
          "ReadRequirement": "Recommended",
          "WriteRequirement": "Recommended"
        },
        "HostName": {
          "ReadRequirement": "Recommended",
          "ConditionalRequirements": [
            {
              "SubordinateToResource": [
                "Manager",
                "EthernetInterfaceCollection"
              ],
              "ReadRequirement": "Mandatory",
              "WriteRequirement": "Mandatory"
            }
          ]
        },
        "FQDN": {
          "ReadRequirement": "Recommended",
          "ConditionalRequirements": [
            {
              "SubordinateToResource": [
                "Manager",
                "EthernetInterfaceCollection"
              ],
              "ReadRequirement": "Mandatory",
              "WriteRequirement": "Mandatory"
            }
          ]
        },
        "NameServers": {
          "ReadRequirement": "Recommended",
          "ConditionalRequirements": [
            {
              "SubordinateToResource": [
                "Manager",
                "EthernetInterfaceCollection"
              ],
              "ReadRequirement": "Mandatory",
              "WriteRequirement": "Mandatory"
            }
          ]
        },
        "IPv4Addresses": {
          "ReadRequirement": "Recommended",
          "ConditionalRequirements": [
            {
              "SubordinateToResource": [
                "Manager",
                "EthernetInterfaceCollection"
              ],
              "ReadRequirement": "Mandatory",
              "WriteRequirement": "Mandatory"
            }
          ],
          "PropertyRequirements": {
            "Address": {},
            "SubnetMask": {},
            "AddressOrigin": {},
            "Gateway": {}
          }
        },
        "IPv4StaticAddresses": {
          "ReadRequirement": "Recommended"
        },
        "IPv6AddressPolicyTable": {
          "ReadRequirement": "Recommended"
        },
        "IPv6StaticAddresses": {
          "ReadRequirement": "Recommended"
        },
        "IPv6StaticDefaultGateways": {
          "ReadRequirement": "Recommended"
        },
        "IPv6Addresses": {
          "ReadRequirement": "IfImplemented",
          "PropertyRequirements": {
            "Address": {},
            "PrefixLength": {},
            "AddressOrigin": {},
            "AddressState": {}
          }
        },
        "StaticNameServers": {
          "ReadRequirement": "Recommended"
        }
      }
    },
    "EthernetInterfacesCollection": {
      "PropertyRequirements": {
        "Members": {
          "MinCount": 1
        }
      }
    },
    "ManagerAccount": {},
    "ManagerCollection": {
      "PropertyRequirements": {
        "Members": {
          "MinCount": 1
        }
      }
    },
    "Manager": {
      "PropertyRequirements": {
        "ManagerType": {},
        "UUID": {},
        "Status": {},
        "FirmwareVersion": {},
        "NetworkProtocol": {},
        "EthernetInterfaces": {},
        "LogServices": {},
        "Links": {
          "PropertyRequirements": {
            "ManagerForServers": {
              "ReadRequirement": "IfImplemented"
            },
            "ManagerForChassis": {
              "ReadRequirement": "IfImplemented"
            }
          }
        }
      },
      "ActionRequirements": {
        "Reset": {
          "Parameters": {
            "ResetType": {
              "MinSupportValues": [
                "ForceRestart"
              ]
            }
          }
        }
      }
    },
    "ManagerNetworkProtocol": {
      "PropertyRequirements": {
        "HostName": {},
        "Status": {},
        "FQDN": {},
        "HTTP": {
          "PropertyRequirements": {
            "ProtocolEnable": {},
            "Port": {}
          }
        },
        "HTTPS": {
          "PropertyRequirements": {
            "ProtocolEnable": {},
            "Port": {}
          }
        },
        "SSH": {
          "ReadRequirement": "Recommended",
          "PropertyRequirements": {
            "ProtocolEnable": {},
            "Port": {}
          }
        },
        "SSDP": {
          "ReadRequirement": "Recommended",
          "PropertyRequirements": {
            "ProtocolEnable": {},
            "Port": {},
            "NotifyMulticstIntervalSeconds": {},
            "NotifyIPv6Scope": {},
            "NotifyTTL": {}
          }
        }
      }
    },
    "Power": {
      "PropertyRequirements": {
        "PowerControl": {
          "PropertyRequirements": {
            "PowerConsumedWatts": {
              "ReadRequirement": "Recommended"
            },
            "PowerCapacityWatts": {
              "ReadRequirement": "Recommended"
            },
            "PowerLimit": {
              "ReadRequirement": "Recommended",
              "PropertyRequirements": {
                "LimitInWatts": {},
                "LimitException": {}
              }
            }
          }
        }
      }
    },
    "SessionService": {},
    "Thermal": {
      "MinVersion": "1.1.0",
      "PropertyRequirements": {
        "Fans": {
          "ReadRequirement": "IfImplemented",
          "PropertyRequirements": {
            "Status": {},
            "Name": {
              "ReadRequirement": "Recommended",
              "Purpose": "Transition from the deprecated 'FanName' to 'Name'."
            },
            "Reading": {
              "ReadRequirement": "Recommended"
            },
            "ReadingUnits": {
              "ReadRequirement": "Recommended"
            }
          }
        },
        "Temperatures": {
          "ReadRequirement": "IfImplemented",
          "PropertyRequirements": {
            "ReadingCelsius": {},
            "UpperThresholdFatal": {
              "ReadRequirement": "Recommended"
            },
            "UpperThresholdCritical": {
              "ReadRequirement": "Recommended"
            },
            "UpperThresholdNonCritical": {
              "ReadRequirement": "Recommended"
            },
            "Status": {}
          }
        },
        "Redundancy": {
          "ReadRequirement": "IfImplemented",
          "PropertyRequirements": {
            "MemberId": {},
            "RedundancySet": {},
            "Mode": {},
            "Status": {},
            "MinNumNeeded": {},
            "MaxNumSupported": {
              "ReadRequirement": "Recommended"
            }
          }
        }
      }
    },
    "LogService": {
      "PropertyRequirements": {
        "Entries": {}
      },
      "ActionRequirements": {
        "ClearLog": {
          "ReadRequirement": "Recommended"
        }
      }
    },
    "LogEntry": {
      "PropertyRequirements": {
        "Created": {},
        "EntryType": {},
        "Message": {},
        "Severity": {}
      }
    },
    "ServiceRoot": {
      "PropertyRequirements": {
        "AccountService": {},
        "UUID": {},
        "RedfishVersion": {},
        "Chassis": {},
        "Managers": {},
        "SessionService": {}
      }
    }
  }
}
"""

Intel_RSD_Compute_Base_1_0_0 = """
{
  "SchemaDefinition": "RedfishInteroperabilityProfile.v1_0_0",
  "ProfileName": "IntelRSDComputeBase",
  "ProfileVersion": "1.0.0",
  "Purpose": "Base profile for Intel RSD Compute. It requires minimal set of API to be supported by service to be recognized as Intel RSD Compute.",
  "ContactInfo": "mateusz.mania@intel.com",
  "OwningEntity": "Intel Corporation",
  "RequiredProfiles": {
    "OCPBaselineHardwareManagement": {
      "MinVersion": "1.0.0",
      "Repository": "https://www.opencompute.org/wiki/Hardware_Management/SpecsAndDesigns"
    }
  },
  "Protocol": {
    "MinVersion": "1.0"
  },
  "Resources": {
    "Chassis": {
      "Purpose": "Used to model physical location of the asset, casing topology and information.",
      "PropertyRequirements": {
        "Links": {
          "PropertyRequirements": {
            "ComputerSystems": {
              "Purpose": "Used to create physical topology and relations."
            },
            "ManagersInChassis": {
              "Purpose": "Used to create physical topology and relations."
            }
          }
        }
      }
    },
    "ChassisCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Chassis, a casing for the whole hardware.",
          "MinCount": 1
        }
      }
    },
    "Manager": {
      "Purpose": "Used to model management asset. Managers is used during service registration process and to control service.",
      "PropertyRequirements": {
        "Links": {
          "PropertyRequirements": {
            "ManagerForServers": {
              "Purpose": "Used to know which Computer System is managed by this Manager."
            }
          }
        }
      }
    },
    "ManagerCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Manager, a software exposing Intel RSD Compute.",
          "MinCount": 1
        }
      }
    },
    "ComputerSystem": {
      "Purpose": "Used to model and manage server's configuration.",
      "PropertyRequirements": {
        "PowerState": {
          "Purpose": "Used to proper control state of the Computer System."
        },
        "Boot": {
          "Purpose": "Used to control Computer System's BIOS boot options for composition and/or decomposition.",
          "PropertyRequirements": {
            "BootSourceOverrideEnabled": {},
            "BootSourceOverrideTarget": {},
            "BootSourceOverrideMode": {}
          }
        },
        "Memory": {
          "Purpose": "Computer System must have at least one Memory module to boot up.",
          "MinCount": 1
        },
        "Processors": {
          "Purpose": "Computer System must have at least one Processor to boot up.",
          "MinCount": 1
        },
        "UUID": {
          "Purpose": "Used to recognize Computer System. UUID must be permanent and available from the service boot. Computer Systems with the same UUIDs will be treated as one."
        },
        "NetworkInterfaces": {
          "Purpose": "Computer System must have at least one Network Interface with MAC address to support features like remote boot.",
          "MinCount": 1
        }
      },
      "ActionRequirements": {
        "Reset": {
          "Purpose": "Used to control power state of the Computer System.",
          "Parameters": {
            "ResetType": {
              "MinSupportValues": [
                "On",
                "Off"
              ]
            }
          }
        }
      }
    },
    "ComputerSystemCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Computer System to manage.",
          "MinCount": 1
        }
      }
    },
    "EthernetInterface": {
      "Purpose": "Ethernet Interface is used to model network connection and retrieve basic networking properties like MAC address.",
      "MinVersion": "1.0.0",
      "PropertyRequirements": {
        "PermanentMACAddress": {
          "Purpose": "Used to uniquely identify Network Interface."
        }
      }
    },
    "EthernetInterfaceCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Ethernet Interface to manage.",
          "MinCount": 1
        }
      }
    },
    "Processor": {
      "Purpose": "Used in search for Computer System during composition. You cannot compose a computer system without processor.",
      "MinVersion": "1.0.0",
      "PropertyRequirements": {
        "TotalCores": {
          "Purpose": "Used as one of indicators of Computer System performance. Used in search during Computer System."
        },
        "MaxSpeedMhz": {
          "Purpose": "Used as one of indicators of Computer System performance. Used in search during Computer System."
        },
        "Socket": {
          "Purpose": "Used to identify Processor in multi processors Computer Systems."
        }
      }
    },
    "ProcessorCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Processor in Computer System.",
          "MinCount": 1
        }
      }
    },
    "Memory": {
      "Purpose": "Used in search for Computer System during composition. You cannot compose a computer system without memory.",
      "PropertyRequirements": {
        "CapacityMiB": {
          "Purpose": "Used as a indicator of Memory quality."
        }
      }
    },
    "MemoryCollection": {
      "PropertyRequirements": {
        "Members": {
          "Purpose": "Intel RSD Compute service should have at least one Memory module in Computer System.",
          "MinCount": 1
        }
      }
    },
    "ServiceRoot": {
      "Purpose": "Entry point for the whole API. It contains ",
      "PropertyRequirements": {
        "Systems": {
          "Purpose": "Used to retrieve information about Computer Systems."
        },
        "Tasks": {
          "Purpose": "Used to support long time operations as asynchronous calls."
        },
        "Sessions": {
          "Purpose": "Used to support authentication and user sessions."
        },
        "EventService": {
          "Purpose": "Used to support asynchronous event about Intel RSD service like: Computer System is in Critical state."
        }
      }
    }
  }
}
"""

Intel_RSD_Baseline_Compute_0_0_1 = """
{
  "SchemaDefinition": "RedfishInteroperabilityProfile.v1_0_0",
  "ProfileName": "IntelRSDBaselineCompute",
  "ProfileVersion": "0.0.1",
  "Purpose": "RSD 2.4 baseline interoperability profile for a managed compute device.",
  "OwningEntity": "Intel RSD.",
  "ContactInfo": "rsd_profile@intel.com",
  "Protocol": {
    "MinVersion": "1.0",
    "Discovery": "Mandatory"
  },
  "RequiredProfiles": {
    "IntelRSDComputeBase": {
      "MinVersion": "1.0.0"
    }
  },
  "Resources": {
    "Processor": {
      "PropertyRequirements": {
        "Oem": {
          "PropertyRequirements": {
            "Intel_RackScale": {
              "PropertyRequirements": {
                "CurrentConfiguration": {
                    "Purpose": "Used to get current or to set desired Processor configuration.",
                    "WriteRequirement": "Mandatory"
                },
                "Configurations": {
                  "Purpose": "Used to get list of supported Processor configurations.",
                  "MinCount": 1
                }
              }
            }
          }
        }
      }
    }
  }
}
"""
