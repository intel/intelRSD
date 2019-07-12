package com.intel.rsd.resourcemanager.layers.merger

import java.util.Scanner

class ODataType private constructor(odataType: String) {

    companion object {
        val ODATA_TYPE_PATTERN = """#(?<namespace>\w+)\.(v)(?<version>\d+_\d+_\d+)\.(?<type>\w+)""".toRegex()
        const val UNKNOWN = "#UNKNOWN"
        private val VERSION_BASED_COMPARATOR = VersionBasedComparator()

        fun findHigherVersion(v1: ODataType, v2: ODataType): ODataType = arrayListOf(v1, v2).sortedWith(VERSION_BASED_COMPARATOR).last()

        fun oDataTypeOrNull(odataType: String) = when {
            ODATA_TYPE_PATTERN.matches(odataType) -> ODataType(odataType)
            else -> null
        }
    }

    var value: String = odataType
    var nameSpace: String
    var version: String
    var type: String

    init {
        val matchResult = ODATA_TYPE_PATTERN.matchEntire(odataType)!!
        nameSpace = matchResult.groups["namespace"]!!.value
        version = matchResult.groups["version"]!!.value
        type = matchResult.groups["type"]!!.value
    }

    class VersionBasedComparator : Comparator<ODataType> {
        private val areEqual = 0

        override fun compare(o1: ODataType, o2: ODataType): Int {
            if (o1.nameSpace != o2.nameSpace) {
                throw UncomparableResourcesException("Cannot compare @ODataTypes with different namespaces: ${arrayListOf(o1.nameSpace, o2.nameSpace)}")
            }

            if (o1.type != o2.type) {
                throw UncomparableResourcesException("Cannot compare @ODataTypes with different types: ${arrayListOf(o1.type, o2.type)}")
            }

            val scanner1 = Scanner(o1.version).useDelimiter("_")
            val scanner2 = Scanner(o2.version).useDelimiter("_")

            while (scanner1.hasNextInt() && scanner2.hasNextInt()) {
                val value1 = scanner1.nextInt()
                val value2 = scanner2.nextInt()
                if (value1 != value2) {
                    return Integer.compare(value1, value2)
                }
            }
            return areEqual
        }
    }
}
