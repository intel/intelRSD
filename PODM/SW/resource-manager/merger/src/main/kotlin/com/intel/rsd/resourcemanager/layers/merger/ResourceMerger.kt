package com.intel.rsd.resourcemanager.layers.merger

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.databind.node.*
import com.intel.rsd.resourcemanager.layers.merger.ODataType.Companion.UNKNOWN
import com.intel.rsd.resourcemanager.layers.merger.ODataType.Companion.oDataTypeOrNull
import org.slf4j.LoggerFactory
import org.springframework.stereotype.Component

@Component
class ResourceMerger {
    private val log = LoggerFactory.getLogger(ResourceMerger::class.java)

    private val nodeFactory = ObjectMapper().nodeFactory

    private fun createArrayNode() = ArrayNode(nodeFactory)

    /**
     * order of json nodes matters here...
     */
    fun merge(jsonNodes: List<JsonNode>): JsonNode {
        if (jsonNodes.isEmpty()) {
            return NullNode.instance
        }

        val result = jsonNodes.first().deepCopy<ObjectNode>()

        for (node in jsonNodes.slice(1 until jsonNodes.size)) {
            for ((key, value) in node.fields()) {
                when (value) {
                    is ObjectNode -> mergeObjectNode(result, key, value)
                    is ArrayNode -> mergeArrayNode(result, key, value)
                    else -> mergeValueNode(result, key, value as ValueNode)
                }
            }
        }

        return mergeCollection(result)
    }

    private fun mergeCollection(body: JsonNode): JsonNode {
        if (body.get("Members@odata.count") != null) {
            val objectNode = body as ObjectNode
            if (objectNode["Members"] != null) {
                val jsonNodeList = objectNode["Members"].distinctBy { it["@odata.id"].asText() }
                objectNode["Members"] = createArrayNode().addAll(jsonNodeList)
            }
            objectNode.set("Members@odata.count", IntNode(objectNode.at("/Members").size()))
        }
        return body
    }

    private fun mergeValueNode(result: ObjectNode, key: String?, value: ValueNode) {
        when (key) {
            "@odata.type" -> mergeOdataTypeValue(result, key, value as TextNode)
            else ->
                if (result[key] == null || result[key] == NullNode.instance) {
                    result[key] = value
                }
        }
    }

    private fun mergeObjectNode(result: ObjectNode, key: String?, value: JsonNode) {
        if (result[key] == null || result[key] is NullNode) {
            result[key] = value
        } else {
            result[key] = merge(arrayListOf(result[key], value))
        }
    }

    private fun mergeArrayNode(result: ObjectNode, key: String?, value: JsonNode) {
        val values = result[key] as? ArrayNode ?: createArrayNode()
        value.filter { !values.contains(it) }.forEach { values.add(it) }
    }

    private fun mergeOdataTypeValue(result: ObjectNode, name: String?, valueToMerge: TextNode) {
        val currentValue = result[name] as? ValueNode ?: NullNode.instance
        if (currentValue == valueToMerge) return

        var candidate = oDataTypeOrNull(valueToMerge.asText()) ?: return
        var current = oDataTypeOrNull(currentValue.asText()) ?: run {
            result[name] = TextNode(candidate.value)
            return
        }

        try {
            result[name] = TextNode(ODataType.findHigherVersion(current, candidate).value)
        } catch (e: UncomparableResourcesException) {
            log.error("Cannot determine which version is newer. Comparing: {} and {}", current, candidate)
            result[name] = TextNode(UNKNOWN)
        }
    }
}