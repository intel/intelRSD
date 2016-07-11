"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

MAIN_TEMPLATE = """
<!DOCTYPE html>
<head>
    <script>
        function hide_types_descriptions(){
            types_div = document.getElementById('types');
            types_descriptions = types_div.getElementsByTagName("div");

            for (var i=0; i < types_descriptions.length; i++){

                if (types_descriptions.item(i).parentNode == types_div) {
                    types_descriptions.item(i).style.display="none";
                }
            }
        }

        function hide_entities_descriptions(){
            entities_div = document.getElementById('entities');
            entities_descriptions = entities_div.getElementsByTagName("div");

            for (var i=0; i < entities_descriptions.length; i++){

                if (entities_descriptions.item(i).parentNode == entities_div) {
                    entities_descriptions.item(i).style.display="none";
                }
            }
        }

        function hide_all(){
            hide_types_descriptions();
            hide_entities_descriptions();
        }
        function reload_element(){
            div_content = document.getElementById("content");
            div_content.innerHTML = document.getElementById(location.hash).innerHTML;
        }
    </script>
</head>

<body onload="hide_all();" onhashchange="reload_element();">
    <div style="width:1300px;">
    {{render_menu(types, entities)}}
    <div id="content" style="float:right; width:700px; border:1px solid black;">
    </div>
</div>
{{render_types(types)}}
{{render_entities(entities)}}

</body>
</html>
"""
