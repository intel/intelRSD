import time
import os
import subprocess
from include.psme_xml_structure.lui_model_managers.manager_drawer_manager import ManagerDrawerManager
from include.psme_xml_structure.lui_model_managers.initialize import initialize
from include.libs.xmltodict.xmltodict import unparse

RETRIES = 5
INTERFACES_DIR = "/sys/class/net/"
OPERSTATE_UP = "up\n"

def up_interfaces():
    interfaces_all = os.listdir(INTERFACES_DIR)
    interfaces_filtered = [item for item in interfaces_all if item != "lo" and item != "sit0"]
    for interface in interfaces_filtered:
        retries_left = RETRIES
        while retries_left > 0:
            retries_left -= 1
            with open(INTERFACES_DIR + interface + "/operstate") as f:
                operstate = f.readline()
            if operstate == OPERSTATE_UP:
                break
            else:
                subprocess.call( "ifconfig " + interface + " up", shell = True)
                time.sleep(1)

try:
    up_interfaces()

    XML_DEEP_DISCOVERY_FILE = "deep_discovery.xml"
    XML_DEEP_DISCOVERY_PATH = "/usr/bin/"

    initialize()
    agent_emulator_data = ManagerDrawerManager.get_data(None)
    agent_emulator_data = agent_emulator_data.parse_to_dict()
    agent_emulator_data = unparse(agent_emulator_data)

    with open(XML_DEEP_DISCOVERY_PATH + XML_DEEP_DISCOVERY_FILE, "w") as dd_file:
        dd_file.write(str(agent_emulator_data))

except Exception as e:
    import traceback
    print str(traceback.format_exc())
    exit(1)

exit(0)