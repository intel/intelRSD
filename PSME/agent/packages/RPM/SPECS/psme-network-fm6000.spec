Summary: Package for RackScale PSME network
Name: psme-network-fm6000
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
mkdir -p $RPM_BUILD_ROOT/etc/sysconfig/network-scripts
mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-network-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/usr/local/bin/bmc-port-fw.py ./usr/local/bin/
cp -p %{SOURCEURL0}/usr/lib/systemd/system/bmc-port-fw.service ./usr/lib/systemd/system/

%post
if [ ! -f /etc/systemd/system/bmc-port-fw.service ]; then
    ln -s /usr/lib/systemd/system/bmc-port-fw.service /etc/systemd/system/bmc-port-fw.service
fi
systemctl daemon-reload
systemctl enable bmc-port-fw

cat <<EOF >/lib/systemd/system/psme-network.service
[Unit]
Description=Managing PSME network
After=network.target network-online.target

[Service]
Type=simple
Environment="FM_FREEDOM_TRAIL_CONFIG_FILE=/etc/tmcd/fm_platform_attributes.cfg" "FM_API_SHM_KEY=501,restart"
ExecStart=/usr/bin/psme-network /etc/psme/psme-network-configuration.json
PIDFile=/var/run/psme-network.pid

[Install]
WantedBy=multi-user.target
EOF

if [ ! -f /etc/systemd/system/psme-network.service ]; then
    ln -s /lib/systemd/system/psme-network.service /etc/systemd/system/psme-network.service
fi
systemctl daemon-reload
systemctl enable psme-network

%preun
systemctl stop psme-network

%postun
if [ "$1" == 0 ]; then
	systemctl disable psme-network
	if [ -f /etc/systemd/system/psme-network.service ]; then
	    rm /etc/systemd/system/psme-network.service
	fi
	if [ -f /lib/systemd/system/psme-network.service ]; then
	    rm /lib/systemd/system/psme-network.service
	fi
	if [ -f /etc/systemd/system/bmc-port-fw.service ]; then
		rm /etc/systemd/system/bmc-port-fw.service
	fi
	systemctl daemon-reload
else
	exit 0
fi
%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%config /etc/psme/psme-network-configuration.json
%attr(600,root,root) /etc/psme/psme-network-configuration.json
%attr(700,root,root) %{_bindir}/psme-network
/usr/lib/systemd/system/*
/usr/local/bin/*


%changelog
