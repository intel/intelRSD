#! /bin/sh

show_usage() {
    cat <<EOF
This scripts generates self signed SSL certificate if the files in the paths specified do not exist.

Usage: $0 [--srv-cert=<cert-file>] [--srv-key=<key-file>]

Arguments
-c, --srv-cert=path   Write resulting certificate to file. Default is "srv_cert".
-h, --help            Show this help.
-k, --srv-key=path    Write resulting key to file. Default is "srv_key".
EOF
    exit 1
}

gen_cert() {
    key="$1"
    cert="$2"

    srvcsr=$(mktemp srv_csr.XXXXXX)
    trap 'rm -f -- "$srvcsr"' INT TERM HUP EXIT

    openssl ecparam -name secp521r1 -genkey -out "${key}" \
    && openssl req -new -sha256 -key "${key}" -out "${srvcsr}" \
            -subj "/C=EX/O=Example Inc/CN=$(hostname)" \
    && openssl x509 -req -in "${srvcsr}" -signkey "${key}" -out "${cert}"
}

srv_key="srv_key"
srv_cert="srv_cert"

TEMP=$(getopt -o c:hk: --long srv-cert:,help,srv-key: -n $0 -- "$@")
if [ $? != 0 ]; then echo "Terminating..." >&2; exit 1; fi
eval set -- "$TEMP"
while true; do
    case "$1" in
	-c | --srv-cert ) srv_cert="$2"; shift 2;;
	-h | --help ) show_usage; shift;;
	-k | --srv-key ) srv_key="$2"; shift 2;;
	-- ) shift; break;;
	* ) echo "Internal error."; exit 1;;
    esac
done

if [ ! "$(which openssl)" ]; then
    echo "openssl is required for this script to work"
    exit 1
fi

if [ ! -e "$srv_key" -o ! -e "$srv_cert" ]; then
    echo "generating certificate"
    gen_cert "$srv_key" "$srv_cert"
fi
