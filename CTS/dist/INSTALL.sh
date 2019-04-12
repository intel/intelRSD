#!/usr/bin/env bash

# Copyright (c) 2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#             ____ _____ ____
#            / ___|_   _/ ___|
#           | |     | | \___ \
#           | |___  | |  ___) |
#            \____| |_| |____/
#    ___           _        _ _
#   |_ _|_ __  ___| |_ __ _| | | ___ _ __
#    | || '_ \/ __| __/ _` | | |/ _ \ '__|
#    | || | | \__ \ || (_| | | |  __/ |
#   |___|_| |_|___/\__\__,_|_|_|\___|_|
#

# enter the script's directory
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SYS_DEPENDENCIES="python-pip python-setuptools git python-lxml python-dev python-pysqlite2 build-essential libssl-dev libffi-dev"

HOME_DIR=`eval echo ~$SUDO_USER`

DIR_CTS="$HOME_DIR/.cts/"
DIR_CTS_CONFIG="${DIR_CTS}configuration/"
DIR_CTS_DB="${DIR_CTS}db/"
DIR_CTS_TESTS="${DIR_CTS}tests/"
DIR_CTS_TESTS_DATA="${DIR_CTS}tests_data/"
DIR_CTS_PERFORMANCE_DATA="${DIR_CTS}performance_data/"
DIR_CTS_METADATA="${DIR_CTS}metadata/"
DIR_CTS_SCENARIO="${DIR_CTS}scenario/"


CONFIG_INI="configuration.ini"
MAN_DIR="/usr/share/man/man1"
MAN_SOURCE_FILE="utils/cts.1"
MAN_TARGET_FILE="$MAN_DIR/cts.1"
PIP_LOGS="${DIR_CTS}pip-logs"

SQL_CONFIG_INI="SQL_CONNECTION_STRING = sqlite:///${DIR_CTS_DB}sqlalchemy_cts"

# customs tests goes here
DIR_OPT_CTS_TESTS="/opt/cts/tests/"
# logs goes here
DIR_VAR_LOG_CTS="/var/log/cts/"

DIR_ARRAY=("$DIR_OPT_CTS_TESTS" "$DIR_VAR_LOG_CTS" "$DIR_CTS_CONFIG" "$DIR_CTS_DB" "$DIR_CTS_TESTS" "$DIR_CTS_TESTS_DATA" "$DIR_CTS_METADATA" "$DIR_CTS_SCENARIO" "$DIR_CTS_PERFORMANCE_DATA")
DIR_ARRAY_LEN=${#DIR_ARRAY[@]}

DIR_ARRAY_WO_DB=("$DIR_CTS_TESTS" "$DIR_CTS_METADATA")
DIR_ARRAY_WO_DB_LEN=${#DIR_ARRAY_WO_DB}

if pip -V | cut -d ' ' -f 4 | grep -q "/usr/lib/python2.7/dist-packages"; then
    PIP_VERSION="pip"
else
    PIP_VERSION="pip2"
fi


STATUS=0
PROXY=""

function show_cts_logo {
    echo "             ____ _____ ____              "
    echo "            / ___|_   _/ ___|             "
    echo "           | |     | | \___ \             "
    echo "           | |___  | |  ___) |            "
    echo "            \____| |_| |____/             "
    echo "    ___           _        _ _            "
    echo "   |_ _|_ __  ___| |_ __ _| | | ___ _ __  "
    echo "    | || '_ \/ __| __/ _, | | |/ _ \ '__| "
    echo "    | || | | \__ \ || (_| | | |  __/ |    "
    echo "   |___|_| |_|___/\__\__,_|_|_|\___|_|    "
    echo ""
    echo "    Rack Scale Design by Intel (R) 2019   "
    echo ""
}

function exiting {
    echo "  Exiting..."
    exit
}

# Check that run on Linux
if [ $OSTYPE != "linux-gnu" ]
then
    echo "This installer works only with 'linux-gnu' OS type."
    exiting
fi

# Check that script was ran with sudo
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

function show_menu {
    echo "Usage : Choose from the options below:"
    echo ""
    echo "RECOMENDED:"
    echo " 1) Install ALL [CTS, PIP dep., autocompletion & tests]"
    echo ""
    echo "OPTIONAL:"
    echo " 2) Install all PIP's dependencies"
    echo " 3) Install autocompletion for bash (works only with Ubuntu)"
    echo " 4) Install CTS manpage"
    echo " 6) Upgrade CTS"
    echo " 8) Repair ownership"
    echo " 9) Set proxy for current installation"
    echo " 0) Install/Upgrade OS dependencies (Ubuntu only)"
    echo ""
    echo "TROUBLESHOOTING:"
    echo " U) Uninstall CTS"
    echo " W) WIPE ALL YOUR DATA and uninstall CTS"
    echo ""
    echo " E) Close installer"
    echo ""
    read -p "Input Selection: " menu_option
    main_menu $menu_option

}

function show_available_arguments {
    echo "   1|-f|--full-install)     Install ALL [CTS, PIP dep., autocompletion & tests]"
    echo "   2|-p|--pip-install)      Install all PIP's dependencies"
    echo "   3|-a|--autocompletion)   Install autocompletion for bash (works only with Ubuntu)"
    echo "   4|-m|--manpage)          Install CTS manpage"
    echo "   6|-u|--upgrade)          Upgrade CTS"
    echo "   8|-r|--repair)           Repair ownership"
    echo "   9|-P|--proxy)            Set proxy for current installation"
    echo "   0|-S|--system)           Install/Upgrade OS dependencies (Ubuntu only)"
    echo "   U|-U|--Uninstall)        Uninstall CTS"
    echo "   W|-W|--WipeAllData       WIPE ALL YOUR DATA and uninstall CTS"
    echo ""
    echo "     -i|--interactive       Show advanced mode"
    echo "   --FactoryInstall)        WIPE ALL DATA and perform a CTS reinstall"
    echo "   --FactoryUninstall)      WIPE ALL DATA and uninstall CTS (WARNING! this option has no confirmation)"
    echo ""
}


function install_apt_get_updates {
    apt-get -y update > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Problem with apt-get update, check your internet connection"
        exiting
    fi
    apt-get -y install $SYS_DEPENDENCIES > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Problem with apt-get update, check your internet connection"
        exiting
    fi

}

function set_proxy {
    read -p "    Please enter proxy to use: " proxy
    echo ""
    if [ ! -z "$proxy" ]; then
         PROXY="--proxy ${proxy}"
    fi
}

function install_pip_dependencies {
    sudo -H ${PIP_VERSION} --default-timeout=5 --retries 0 install ${PROXY} --log "$PIP_LOGS" -r utils/requirements.txt > /dev/null
    sudo -H ${PIP_VERSION} --default-timeout=5 --retries 0 install  ${PROXY} --log "$PIP_LOGS" -r utils/requirements-no-deps.txt --no-deps > /dev/null
}

function install_autocompletion_for_bash {
    linux_dist="$(lsb_release -i -s)"
    if [ $linux_dist == "Ubuntu" ] ; then
        /bin/cp -ruf utils/bash_autocompletion.sh /etc/bash_completion.d/cts
        echo "complete -F _cts cts" >> $HOME_DIR/.bashrc
        source /etc/profile
        source $HOME_DIR/.bashrc
        echo -ne "Done\r"
    else
        echo "  !! Module wasn't installed, works only with Ubuntu OS"
    fi
}

function install_manpage {
    /bin/cp -ruf $MAN_SOURCE_FILE $MAN_TARGET_FILE
    gzip -f $MAN_TARGET_FILE
    # echo -e "\nUpdating man-db..."
    mandb > /dev/null 2> /dev/null
}

function create_needed_dirs {
    for (( i=0; i<${DIR_ARRAY_LEN}; i++ ));
    do
        if [ ! -d ${DIR_ARRAY[$i]} ] ; then
            mkdir -p ${DIR_ARRAY[$i]}
            # echo "    > Creating ${DIR_ARRAY[$i]}"
        fi
    done
}

function remove_old_tests {
    for (( i=0; i<${DIR_ARRAY_WO_DB_LEN}; i++ ));
    do
        if [ -d ${DIR_ARRAY_WO_DB[$i]} ] ; then
            rm -rf ${DIR_ARRAY_WO_DB[$i]}
            # echo "    > Deleting ${DIR_ARRAY_WO_DB[$i]}"
        fi
    done
}

function remove_ALL_folders {
    for (( i=0; i<${DIR_ARRAY_LEN}; i++ ));
    do
        if [ -d ${DIR_ARRAY[$i]} ] ; then
            sudo rm -rf ${DIR_ARRAY[$i]}
            # echo "    > Deleting ${DIR_ARRAY[$i]}"
        fi
    done
}

function copy_configuration_ini {
    cp $CONFIG_INI ${DIR_CTS_CONFIG}${CONFIG_INI}
    if [ $? -ne 0 ] ; then
        STATUS=1
        echo "  !! Can't copy configuration"
    else
        echo "$SQL_CONFIG_INI" >> ${DIR_CTS_CONFIG}${CONFIG_INI}
    fi
}

function copy_tests {
    # echo "    > Copy tests_packages/* to $DIR_CTS_TESTS"
    sudo -u $SUDO_USER cts tests dump
    chmod a+x -R $DIR_CTS
}

function change_ownership_for_sudo_user {
    for (( i=0; i<${DIR_ARRAY_LEN}; i++ ));
    do
        chown -R $SUDO_USER ${DIR_ARRAY[$i]}
    done
}

function repair_ownership {
    read -p "    Please enter username for executing CTS: " username
    echo ""
    create_needed_dirs
    copy_tests
    chown -R ${username} ${DIR_CTS}*
    for (( i=0; i<${DIR_ARRAY_LEN}; i++ ));
    do
        chown -R ${username} ${DIR_ARRAY[$i]}
        if [ $? != 0 ] ; then
            echo "    User {${username}} doesn't' exist'"
            repair_ownership
        fi
    done
}

function cts_upgrade {
    cd utils/
    python pip_deps_checker.py
    if [ $? -ne 0 ] ; then
        echo ""
        echo "Exiting. Missing Python dependencies"
        exit 1
    fi
    cd ..

    PREVIOUS_VERSION="$(${PIP_VERSION} freeze | grep CTS | sed 's/.*=//')"
    NEW_VERSION="$(find . -name CTS* | cut -d "-" -f2)"


    if [[ "$(printf "${PREVIOUS_VERSION}\n${NEW_VERSION}" | sort -V | head -n1)" == "${NEW_VERSION}" ]]; then
        echo "Installed version is ${PREVIOUS_VERSION}, while version to install is ${NEW_VERSION}. Nothing to update, exiting..."
        exit 1
    elif [[ "${PREVIOUS_VERSION}" != "" ]]; then
        echo "Installed version is ${PREVIOUS_VERSION}, while version to install is ${NEW_VERSION}. Begginging update."
    fi

    ${PIP_VERSION} install --log "$PIP_LOGS" CTS* > /dev/null 2>&1
    if [ $? -ne 0 ] ; then
        STATUS=1
    fi

    if [ ! -f CTS*.whl ] ; then
        STATUS=1
        echo "  !! CTS file don't exist. CTS's file should be in the same directory as INSTALL.sh"
    fi
}

function cts_uninstall {
    echo "    CTS will be uninstalled"
    read -n 1 -p "    Are you sure? (y/n)" option
    if [ $option == "y" ] ; then
        sudo -H ${PIP_VERSION} uninstall --log "$PIP_LOGS" cts -y
        rm "$MAN_TARGET_FILE.gz" 2> /dev/null
    elif [ $option == "n" ] ; then
        echo "    Okay, CTS will not be uninstalled"
    else
        echo "    This wasn't correct key"
    fi
}

function cts_wipe_all {
    echo "    All data will be WIPED OUT"
    read -n 1 -p "    Are you sure? (y/n)" option
    if [ $option == "y" ] ; then
        sudo -H ${PIP_VERSION} uninstall --log "$PIP_LOGS" cts -y
        remove_ALL_folders
    elif [ $option == "n" ] ; then
        echo "    Okay, data will NOT wiped out"
    else
        echo "    This wasn't correct key"
    fi
}

function cts_wipe_all_wo_prompt {
    echo "    All data will be WIPED OUT"
    sudo -H ${PIP_VERSION} uninstall --log "$PIP_LOGS" cts -y
    remove_ALL_folders
}

function clean_cts_install_prompt {
    echo "    This option installs a fresh CTS. All old data will be WIPED OUT"
    read -p "    Are you sure? (Y/n)" option
    if [ $option == "Y" ] ; then
        clean_cts_install
    elif [ $option == "n" ] ; then
        echo "    Okay, CTS will not be installed"
    else
        echo "    This wasn't correct key"
    fi
}

function cts_test_run {
    cts version
    if [ $? != 0 ] ; then
        echo "    [ERROR] CTS was not installed correctly"
        exit 127
    fi
}

function clean_cts_install {
    status=0
    echo -ne "Working: __________ (0%)\r"

    # install all PIP dependencies
    echo -ne "Working: #_________ (12%)\r"
    install_pip_dependencies
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    # remove only old tests
    remove_old_tests
    create_needed_dirs
    echo -ne "Working: ##________ (24%)\r"
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    # unlock ownership
    change_ownership_for_sudo_user
    echo -ne "Working: ####______ (48%)\r"
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    # install included CTS package
    cts_upgrade
    echo -ne "Working: ######____ (60%)\r"
    if [ $? -ne 0 ] ; then
        STATUS=1
    fi

    # copy configuration.ini to /etc/cts/
    copy_configuration_ini
    echo -ne "Working: #######___ (75%)\r"
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    # copy test_packages to /opt/cts/tests/
    copy_tests
    echo -ne "Working: #########_ (91%)\r"
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    echo -ne "Working: ########## (100%)\r"
    install_manpage
    install_autocompletion_for_bash

    # repair ownership for folders
    change_ownership_for_sudo_user
    if [ $? != 0 ] ; then
        STATUS=1
    fi

    echo -ne "                               \r"
    # cts version
    cts_test_run

    if [ $STATUS -eq 0 ] ; then
        echo "  Done. CTS installed correctly."
        exit
    else
        echo "  ERROR ${STATUS}. Try install CTS again."
        exit 1
    fi
}


main_menu () {
    if [ -z $1 ] ; then
        menu_option="1"
    else
        menu_option=$1
    fi

    case ${menu_option} in
    1|-f|--full-install)
        echo "  Install ALL [CTS, PIP dep., autocompletion, manpage & tests]"
        clean_cts_install
        exiting ;;

    2|-p|--pip-install)
        echo "  Install all PIP's dependencies"
        install_pip_dependencies
        exit ;;

    3|-a|--autocompletion)
        echo "  Install autocompletion for bash (works only with Ubuntu)"
        install_autocompletion_for_bash
        exit ;;

    4|-m|--manpage)
        echo "  Install CTS manpage"
        install_manpage
        exit ;;

    6|-u|--upgrade)
        echo "  Upgrade CTS"
        cts_upgrade
        remove_old_tests
        copy_tests
        repair_ownership
        install_manpage
        exit ;;

    8|-r|--repair)
        echo "  Repair ownership"
        repair_ownership
        exit ;;

    9|-P|--proxy)
        set_proxy
        show_menu
        exit;;

    0|-S|--system)
        echo "  Install/Upgrade OS dependencies (Ubuntu only)"
        install_apt_get_updates
        exit;;

    U|-U|--Uninstall)
        echo "  Uninstall CTS"
        cts_uninstall
        exit ;;

    W|-W|--WipeAllData)
        echo "  WIPE ALL DATA and uninstall CTS"
        cts_wipe_all
        exit ;;

    -i|--interactive)
        show_menu
        exit;;

    --FactoryUninstall)
        echo "**** Factory Uninstall CTS ****"
        sudo -H ${PIP_VERSION} uninstall --log "$PIP_LOGS" cts -y
        rm "$MAN_TARGET_FILE.gz" 2> /dev/null
        remove_ALL_folders
        exit;;

    --FactoryInstall)
        echo "**** Factory Install ****"
        cts_wipe_all_wo_prompt
        clean_cts_install
        exit ;;

    E)  echo "  Closing installer..."
        exit ;;

    H|-h|--help)
        show_available_arguments
        exit ;;

    *)  echo ""
        show_menu
        main_menu ;;
    esac
}

# Main program
show_cts_logo
main_menu $1
