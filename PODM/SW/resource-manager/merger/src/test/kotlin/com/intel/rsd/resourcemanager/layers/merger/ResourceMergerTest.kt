package com.intel.rsd.resourcemanager.layers.merger

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.databind.node.NullNode
import org.intellij.lang.annotations.Language
import org.testng.Assert.assertEquals
import org.testng.annotations.Test

class ResourceMergerTest {

    @Test
    fun `merged null resource is a null resource`() {
        val sut = ResourceMerger()
        val actual = sut.merge(emptyList())

        assertEquals(actual, NullNode.instance)
    }

    @Test
    fun `property is set in two resources - should equal to the first one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "SerialNumber": "first"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "SerialNumber": "second"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "SerialNumber": "first"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `property is not present in first resource but is in second`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "SerialNumber": "first"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "SerialNumber": "second",
            "SerialNumber2": "second"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "SerialNumber": "first",
            "SerialNumber2": "second"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `null in first but not in second`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "SerialNumber": null
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "SerialNumber": "second"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "SerialNumber": "second"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `nested properties`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "Location": {
              "Id": null,
              "ParentId": "d"
            }
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
          "Location": {
              "Id": "okolo",
              "ParentId": null
            }
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
          "Location": {
              "Id": "okolo",
              "ParentId": "d"
            }
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `handle arrays`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "Flags": [
              "a",
              "b"
            ]
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
             "Flags": [
              "c",
              "d"
            ]
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
             "Flags": [
              "a",
              "b",
              "c",
              "d"
            ]
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }


    @Test
    fun `handle arrays of links`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "LogicalDrives": [
              { "@odata.id": "/redfish/v1/LogicalDrives/abc" },
              { "@odata.id": "/redfish/v1/LogicalDrives/def" }
            ]
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "LogicalDrives": [
              { "@odata.id": "/redfish/v1/LogicalDrives/123" },
              { "@odata.id": "/redfish/v1/LogicalDrives/456" }
            ]
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "LogicalDrives": [
              { "@odata.id": "/redfish/v1/LogicalDrives/abc" },
              { "@odata.id": "/redfish/v1/LogicalDrives/def" },
              { "@odata.id": "/redfish/v1/LogicalDrives/123" },
              { "@odata.id": "/redfish/v1/LogicalDrives/456" }
            ]
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `object node in first but null in second`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """{
          "ContainedBy": {
            "@odata.id": "/redfish/v1/Chassis/1"
          }
        }
        """.toJsonNode()
        @Language("JSON")
        val second = """{"ContainedBy": null }""".toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, first)
    }

    @Test
    fun `array node in first but null in second`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """{
          "Contains": [
            {"@odata.id": "/redfish/v1/Chassis/1"},
            {"@odata.id": "/redfish/v1/Chassis/2"}
          ]
        }
        """.toJsonNode()

        @Language("JSON")
        val second = """{"Contains": null }""".toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, first)
    }

    @Test
    fun `merge collection`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "Members": [
              { "@odata.id": "/redfish/v1/Chassis/abc" },
              { "@odata.id": "/redfish/v1/Chassis/abc" },
              { "@odata.id": "/redfish/v1/Chassis/def" }
            ],
            "Members@odata.count": 2
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "Members": [
              { "@odata.id": "/redfish/v1/Chassis/123" },
              { "@odata.id": "/redfish/v1/Chassis/456" }
            ],
            "Members@odata.count": 2
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "Members": [
              { "@odata.id": "/redfish/v1/Chassis/abc" },
              { "@odata.id": "/redfish/v1/Chassis/def" },
              { "@odata.id": "/redfish/v1/Chassis/123" },
              { "@odata.id": "/redfish/v1/Chassis/456" }
            ],
            "Members@odata.count": 4
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `merge collection Members odata count is present but Members property is missing`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "Members@odata.count": 0
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "Members@odata.count": 0
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "Members@odata.count": 0
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `property present in second but not in first`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """{
          "Actions": {
            "#ComputerSystem.Reset": {
              "target": "/redfish/v1/Systems/1/Actions/ComputerSystem.Reset",
              "ResetType@Redfish.AllowableValues": [
                "On",
                "ForceOff",
                "GracefulShutdown",
                "ForceRestart",
                "Nmi"
            ]},
            "Oem": {
                "#Intel.Oem.EraseOptaneDCPersistentMemory": {
                  "target": "/redfish/v1/Systems/1/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
              }
            }
          }
        } """.toJsonNode()

        @Language("JSON")
        val second = """{
          "Actions": {
            "#ComputerSystem.Reset": {
              "target": "/redfish/v1/Systems/1/Actions/ComputerSystem.Reset",
              "ResetType@Redfish.AllowableValues": [
                "GracefulRestart",
                "ForceOn",
                "PushPowerButton"
              ]
            },
            "Oem": {
              "Intel_RackScale": {
                "#Intel.Oem.EraseOptaneDCPersistentMemory": {
                  "target": "/redfish/v1/Systems/1/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
                }
              }
            }
          }
        } """.toJsonNode()

        @Language("JSON")
        val expected = """{
          "Actions": {
            "#ComputerSystem.Reset": {
              "target": "/redfish/v1/Systems/1/Actions/ComputerSystem.Reset",
              "ResetType@Redfish.AllowableValues": [
                "On",
                "ForceOff",
                "GracefulShutdown",
                "ForceRestart",
                "Nmi",
                "GracefulRestart",
                "ForceOn",
                "PushPowerButton"
              ]
            },
            "Oem": {
              "Intel_RackScale": {
                "#Intel.Oem.EraseOptaneDCPersistentMemory": {
                  "target": "/redfish/v1/Systems/1/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
                }
              },
              "#Intel.Oem.EraseOptaneDCPersistentMemory": {
                  "target": "/redfish/v1/Systems/1/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"
              }
            }
          }
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `handle arrays of embedded resources (with JSON pointers)`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "PowerSupplies": [
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/0",
                  "MemberId": "3"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/1",
                  "MemberId": "4"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/2",
                  "MemberId": "1"
                }
            ]
        } """.toJsonNode()

        @Language("JSON")
        val second = """{
          "PowerSupplies": [
            {
              "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/3",
              "MemberId": "5"
            },
            {
              "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/4",
              "MemberId": "7"
            },
            {
              "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/5",
              "MemberId": "6"
            }
          ]
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "PowerSupplies": [
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/0",
                  "MemberId": "3"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/1",
                  "MemberId": "4"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/2",
                  "MemberId": "1"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/3",
                  "MemberId": "5"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/4",
                  "MemberId": "7"
                },
                {
                  "@odata.id": "/redfish/v1/Chassis/64/Power#/PowerSupplies/5",
                  "MemberId": "6"
                }
            ]
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `@odatatype is set in two resources, second resource defines higher version - should equal to the second one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "@odata.type": "#ComputerSystem.v1_1_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "@odata.typer": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `@odatatype is set in two resources, first resource defines higher version - should equal to the first one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "@odata.type": "#ComputerSystem.v1_1_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "@odata.typer": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `@odatatype is set in first resource only - should equal to the first one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {} """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "@odata.typer": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `@odatatype is set in second resource only - should equal to the second one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {} """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }

    @Test
    fun `when more than two nodes are provided and third one has the newest version - should equal to the third one`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {} """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "@odata.type": "#ComputerSystem.v1_11_0.ComputerSystem"
        } """.toJsonNode()


        @Language("JSON")
        val third = """ {
            "@odata.type": "#ComputerSystem.v9_11_0.ComputerSystem"
        } """.toJsonNode()


        @Language("JSON")
        val expected = """ {
            "@odata.type": "#ComputerSystem.v9_11_0.ComputerSystem"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second, third))

        assertEquals(actual, expected)
    }

    @Test
    fun `there are two different @odatatypes - should equal UNKNOWN`() {
        val sut = ResourceMerger()

        @Language("JSON")
        val first = """ {
            "@odata.type": "#Czarek.v1_11_0.Czarek"
        } """.toJsonNode()

        @Language("JSON")
        val second = """ {
            "@odata.type": "#Marek.v1_11_0.Marek"
        } """.toJsonNode()

        @Language("JSON")
        val expected = """ {
            "@odata.type": "#UNKNOWN"
        } """.toJsonNode()

        val actual = sut.merge(arrayListOf(first, second))

        assertEquals(actual, expected)
    }
}

private fun String.toJsonNode(): JsonNode = ObjectMapper().readTree(this)
