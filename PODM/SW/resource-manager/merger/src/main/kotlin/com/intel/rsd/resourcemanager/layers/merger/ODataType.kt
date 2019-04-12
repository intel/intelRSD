package com.intel.rsd.resourcemanager.layers.merger

import java.util.Scanner

class ODataType {

    companion object {
        val ODATA_TYPE_PATTERN = """#(?<namespace>\w+)\.(v)(?<version>\d+_\d+_\d+)\.(?<type>\w+)""".toRegex()
        val UNKNOWN = "#UNKNOWN"
        val VERSION_BASED_COMPARATOR = VersionBasedComparator()

        fun findHigherVersion(v1: ODataType, v2: ODataType): ODataType = arrayListOf(v1, v2).sortedWith(VERSION_BASED_COMPARATOR).last()

        fun oDataTypeOrNull(odataType: String) = when {
            ODATA_TYPE_PATTERN.matches(odataType) -> ODataType(odataType)
            else -> null
        }
    }

    var value: String
    var nameSpace: String
    var version: String
    var type: String

    private constructor(odataType: String) {
        this.value = odataType
        val matchResult = ODATA_TYPE_PATTERN.matchEntire(odataType)!!;
        nameSpace = matchResult.groups.get("namespace")!!.value
        version = matchResult.groups.get("version")!!.value
        type = matchResult.groups.get("type")!!.value

    }

    class VersionBasedComparator : Comparator<ODataType> {
        val ARE_EQUAL = 0

        override fun compare(o1: ODataType, o2: ODataType): Int {
            if (!o1.nameSpace.equals(o2.nameSpace)) {
                throw UncomparableResourcesException("Cannot compare @ODataTypes with different namespaces: ${arrayListOf(o1.nameSpace, o2.nameSpace)}")
            }

            if (!o1.type.equals(o2.type)) {
                throw UncomparableResourcesException("Cannot compare @ODataTypes with different types: ${arrayListOf(o1.type, o2.type)}")
            }

            val scaner1 = Scanner(o1.version).useDelimiter("_")
            val scaner2 = Scanner(o2.version).useDelimiter("_")

            while (scaner1.hasNextInt() && scaner2.hasNextInt()) {
                val value1 = scaner1.nextInt()
                val value2 = scaner2.nextInt()
                if (value1 != value2) {
                    return Integer.compare(value1, value2)
                }
            }
            return ARE_EQUAL
        }
    }
}
