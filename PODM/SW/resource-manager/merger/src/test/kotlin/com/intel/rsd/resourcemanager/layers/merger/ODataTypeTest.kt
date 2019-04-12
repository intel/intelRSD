package com.intel.rsd.resourcemanager.layers.merger

import com.intel.rsd.resourcemanager.layers.merger.ODataType.Companion.oDataTypeOrNull
import org.assertj.core.api.Assertions.assertThat
import org.junit.Assert.assertNotNull
import org.testng.annotations.DataProvider
import org.testng.annotations.Test

class ODataTypeTest {

    @Test(dataProvider = "odataTypes")
    fun testFindHigher(odataType1: String, odataType2: String, expectedOdataType: String) {
        val v1 = oDataTypeOrNull(odataType1)
        val v2 = oDataTypeOrNull(odataType2)
        assertNotNull(v1)
        assertNotNull(v2)
        assertThat(ODataType.findHigherVersion(v1!!, v2!!).value).isEqualTo(expectedOdataType)
    }

    @DataProvider(name = "odataTypes")
    fun odataTypes(): Array<Array<Any?>> {
        return arrayOf(
                arrayOf<Any?>("#ComputerSystem.v1_1_0.ComputerSystem", "#ComputerSystem.v1_1_0.ComputerSystem", "#ComputerSystem.v1_1_0.ComputerSystem"),
                arrayOf<Any?>("#ComputerSystem.v2_1_0.ComputerSystem", "#ComputerSystem.v1_1_0.ComputerSystem", "#ComputerSystem.v2_1_0.ComputerSystem"),
                arrayOf<Any?>("#ComputerSystem.v2_9_0.ComputerSystem", "#ComputerSystem.v2_8_0.ComputerSystem", "#ComputerSystem.v2_9_0.ComputerSystem"),
                arrayOf<Any?>("#ComputerSystem.v1_11_0.ComputerSystem", "#ComputerSystem.v1_1_0.ComputerSystem", "#ComputerSystem.v1_11_0.ComputerSystem")
        )
    }
}
