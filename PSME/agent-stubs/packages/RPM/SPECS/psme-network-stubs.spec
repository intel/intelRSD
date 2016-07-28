Summary: Package for RSA PSME network stubs
Name: psme-network-stubs
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/etc/psme
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-network-stubs-configuration.json ./etc/psme/

%pre
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-network-stubs-configuration.json ]; then
		cp /etc/psme/psme-network-stubs-configuration.json /etc/psme/psme-network-stubs-configuration.json.old
	fi
else
	exit 0
fi


%post


cat <<EOF >/lib/systemd/system/psme-network-stubs.service
[Unit]
Description=Managing PSME network stubs
After=network.target network-online.target

[Service]
Type=simple
Environment="FM_FREEDOM_TRAIL_CONFIG_FILE=/etc/tmcd/fm_platform_attributes.cfg" "FM_API_SHM_KEY=501,restart"
ExecStart=/usr/bin/psme-network-stubs /etc/psme/psme-network-stubs-configuration.json
PIDFile=/var/run/psme-network-stubs.pid

[Install]
WantedBy=multi-user.target
EOF

if [ ! -f /etc/systemd/system/psme-network-stubs.service ]; then
    ln -s /lib/systemd/system/psme-network-stubs.service /etc/systemd/system/psme-network-stubs.service
fi
systemctl daemon-reload
systemctl enable psme-network-stubs

%preun
systemctl stop psme-network-stubs

%postun
if [ "$1" == 0 ]; then
	systemctl disable psme-network-stubs
	if [ -f /etc/systemd/system/psme-network-stubs.service ]; then
	    rm /etc/systemd/system/psme-network-stubs.service
	fi
	if [ -f /lib/systemd/system/psme-network-stubs.service ]; then
	    rm /lib/systemd/system/psme-network-stubs.service
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/*
/etc/psme/*

%changelog
