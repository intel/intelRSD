FROM alpine:3.9
RUN apk add --update --no-cache \
        dhcp>=4.4.1-r1 python3>=3.6.8-r2

#TODO: DHCP configuration could be different on different nodes?

COPY ./parse_leases.py /usr/bin/parse_leases.py
COPY ./entrypoint.sh /app/entrypoint.sh

RUN mkdir -p /var/lib/dhcp
RUN touch /var/lib/dhcp/dhcpd.leases

EXPOSE 67/udp
ENTRYPOINT ["/app/entrypoint.sh"]

