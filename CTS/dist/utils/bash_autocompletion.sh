_cts()
{
        local cur prev opts
        COMPREPLY=()
        cur="${COMP_WORDS[COMP_CWORD]}"
        prev="${COMP_WORDS[COMP_CWORD-1]}"

        CTS_PARAMS_SHORT="-h"
        CTS_PARAMS="--help"

        CTS_NAME="cts"
        CTS_MAIN_OPTION="execute tests version sos status replay metadata"

        if [[ ${cur} == --* ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_PARAMS}" -- ${cur}))
                return 0
        fi

        CTS_OPTION_EXECUTE="execute"
        if [[ ${prev} == ${CTS_NAME} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_MAIN_OPTION}" -- ${cur}) )
                return 0
        fi

        CTS_MAIN_EXECUTE="run_list tests"
        if [[ ${prev} == ${CTS_OPTION_EXECUTE} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_MAIN_EXECUTE}" -- ${cur}) )
                return 0
        fi

        CTS_OPTION_RUN_LIST="run_list"
        CTS_RUN_LIST_OPTIONS="-h -g -x"
        if [[ ${prev} == ${CTS_OPTION_RUN_LIST} ]] && [[ ${cur} == -* ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_RUN_LIST_OPTIONS}" -- ${cur}) )
                return 0
        fi

        CTS_OPTION_TESTS="tests"
        CTS_TESTS_OPTIONS="-h -s -t -c -T -g -x"
        if [[ ${prev} == ${CTS_OPTION_TESTS} ]] && [[ ${cur} == -* ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_TESTS_OPTIONS}" -- ${cur}) )
                return 0
        fi

        CTS_OPTION_TEST="tests"
        if [[ ${prev} == ${CTS_NAME} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_OPTION_TEST}" -- ${cur}) )
                return 0
        fi

        CTS_MAIN_TEST_EXECUTE="list show generate_config dump"
        if [[ ${prev} == ${CTS_OPTION_TEST} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_MAIN_TEST_EXECUTE}" -- ${cur}) )
                return 0
        fi

        CTS_OPTION_STATUS="status"
        if [[ ${prev} == ${CTS_NAME} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_OPTION_STATUS}" -- ${cur}) )
                return 0
        fi

        CTS_MAIN_STATUS="list show delete dump"
        if [[ ${prev} == ${CTS_OPTION_STATUS} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_MAIN_STATUS}" -- ${cur}) )
                return 0
        fi

        CTS_OPTION_STATUS="metadata"
        if [[ ${prev} == ${CTS_NAME} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_OPTION_STATUS}" -- ${cur}) )
                return 0
        fi

        CTS_MAIN_METADATA="diff"
        if [[ ${prev} == ${CTS_OPTION_STATUS} ]] ; then
                COMPREPLY=( $(compgen -W "${CTS_MAIN_METADATA}" -- ${cur}) )
                return 0
        fi

}
