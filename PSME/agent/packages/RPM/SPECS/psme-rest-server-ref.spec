Summary: Package for RackScale PSME REST server
Name: psme-rest-server
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ libgcrypt openssl
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}

%filter_from_requires /libgcrypt.*/d
%filter_setup

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/etc/psme
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-rest-server-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/usr/bin/psme-gen-serv-cert.sh ./usr/bin/

%pre
#if this is not the initial installation, then backup old configuration files
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-rest-server-configuration.json ]; then
		cp /etc/psme/psme-rest-server-configuration.json /etc/psme/psme-rest-server-configuration.json.old
	fi
else
	exit 0
fi

%post
mkdir -p /etc/psme/certs

#creates service for psme-rest-server
cat <<EOF >/lib/systemd/system/psme-rest-server.service
[Unit]
Description=Managing PSME rest server
After=network.target network-online.target

[Service]
Type=simple
ExecStartPre=/usr/bin/psme-gen-serv-cert.sh --srv-cert=/etc/psme/certs/server.crt --srv-key=/etc/psme/certs/server.key
ExecStart=/usr/bin/psme-rest-server /etc/psme/psme-rest-server-configuration.json
PIDFile=/var/run/psme-rest-server.pid

[Install]
WantedBy=multi-user.target
EOF
if [ ! -f /etc/systemd/system/psme-rest-server.service ]; then
    ln -s /lib/systemd/system/psme-rest-server.service /etc/systemd/system/psme-rest-server.service
fi

if [ ! -f /etc/ONPSS_VERSION ]; then
	sed -i 's/"enp0s20f0.4094"/"em1"/' /etc/psme/psme-rest-server-configuration.json
fi

systemctl daemon-reload
systemctl enable psme-rest-server

%preun
systemctl stop psme-rest-server

%postun
#if this is uninstallation, then remove services and configuration files
if [ "$1" == 0 ]; then
	systemctl disable psme-rest-server
	if [ -f /etc/systemd/system/psme-rest-server.service ]; then
	    rm /etc/systemd/system/psme-rest-server.service
	fi
	if [ -f /lib/systemd/system/psme-rest-server.service ]; then
	    rm /lib/systemd/system/psme-rest-server.service
	fi
	if [ -f /etc/psme/service_uuid.json ]; then
	    rm /etc/psme/service_uuid.json
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(600,root,root) /etc/psme/psme-rest-server-configuration.json
%attr(700,root,root) %{_bindir}/psme-rest-server
%attr(700,root,root) %{_bindir}/psme-gen-serv-cert.sh

%changelog
