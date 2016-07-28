Summary: Package for RackScale PSME network
Name: psme-network
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ libnl3
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/etc/psme
mkdir -p $RPM_BUILD_ROOT/etc/sysconfig/network-scripts
mkdir -p $RPM_BUILD_ROOT/etc/systemd/network
mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-network-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/bdc-r-network-configuration.json ./etc/psme/
#cp -p %{SOURCEURL0}/etc/sysconfig/network-scripts/ifcfg-enp0s20f0 ./etc/sysconfig/network-scripts/
#cp -p %{SOURCEURL0}/etc/sysconfig/network-scripts/ifcfg-enp0s20f0.170  ./etc/sysconfig/network-scripts/
#cp -p %{SOURCEURL0}/etc/sysconfig/network-scripts/ifcfg-enp0s20f0.4094 ./etc/sysconfig/network-scripts/
cp -p %{SOURCEURL0}/etc/psme/enp0s20f0-vlans-create ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/enp0s20f0-vlans-delete ./etc/psme/
cp -p %{SOURCEURL0}/etc/systemd/network/00-sw0p33.link ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/enp0s20f0.170.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/enp0s20f0.4094.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/enp0s20f0.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p0.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p0.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p25.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p25.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p29.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p29.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p33.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p33.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p37.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p37.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p39.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p39.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p41.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p41.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p43.network ./etc/systemd/network/
cp -p %{SOURCEURL0}/etc/systemd/network/sw0p43.swport ./etc/systemd/network/
cp -p %{SOURCEURL0}/usr/lib/systemd/system/psme-enp0s20f0-vlans.service ./usr/lib/systemd/system/
cp -p %{SOURCEURL0}/usr/local/bin/bmc-port-fw.py ./usr/local/bin/
cp -p %{SOURCEURL0}/usr/lib/systemd/system/bmc-port-fw.service ./usr/lib/systemd/system/

%pre
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-network-configuration.json ]; then
		cp /etc/psme/psme-network-configuration.json /etc/psme/psme-network-configuration.json.old
	fi
	if [ -f /etc/psme/bdc-r-network-configuration.json ]; then
		cp /etc/psme/bdc-r-network-configuration.json /etc/psme/bdc-r-network-configuration.json.old
	fi
	if [ -f /etc/psme/alta-network-configuration.json ]; then
		cp /etc/psme/alta-network-configuration.json /etc/psme/alta-network-configuration.json.old
	fi
else
	exit 0
fi

%post
if [ ! -f /etc/systemd/system/bmc-port-fw.service ]; then
    ln -s /usr/lib/systemd/system/bmc-port-fw.service /etc/systemd/system/bmc-port-fw.service
fi
systemctl daemon-reload
systemctl enable bmc-port-fw

if [ ! -f /etc/ONPSS_VERSION ]; then
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

else

if [ -f /etc/psme/psme-network-configuration.json ]; then
    mv /etc/psme/psme-network-configuration.json /etc/psme/alta-network-configuration.json
fi

if [ -f /etc/psme/bdc-r-network-configuration.json ]; then
    mv /etc/psme/bdc-r-network-configuration.json /etc/psme/psme-network-configuration.json
fi

cat <<EOF >/lib/systemd/system/psme-network.service
[Unit]
Description=Managing PSME network
Wants=fm10kd.service
After=network.target network-online.target psme-enp0s20f0-vlans.service fm10kd.service

[Service]
Type=simple
ExecStart=/usr/bin/psme-network /etc/psme/psme-network-configuration.json
PIDFile=/var/run/psme-network.pid

[Install]
WantedBy=multi-user.target
EOF

if [ ! -f /etc/systemd/system/psme-enp0s20f0-vlans.service ]; then
    ln -s /lib/systemd/system/psme-enp0s20f0-vlans.service /etc/systemd/system/psme-enp0s20f0-vlans.service
fi
systemctl daemon-reload
systemctl enable psme-enp0s20f0-vlans

fi

if [ ! -f /etc/systemd/system/psme-network.service ]; then
    ln -s /lib/systemd/system/psme-network.service /etc/systemd/system/psme-network.service
fi
systemctl daemon-reload
systemctl enable psme-network

%preun
systemctl stop psme-network
if [ "$1" == 0 ]; then
	if [ -f /etc/ONPSS_VERSION ]; then
	    if [ -f /etc/psme/psme-network-configuration.json ]; then
	        mv /etc/psme/psme-network-configuration.json /etc/psme/bdc-r-network-configuration.json
	    fi
	    if [ -f /etc/psme/alta-network-configuration.json ]; then
	        mv /etc/psme/alta-network-configuration.json /etc/psme/psme-network-configuration.json
	    fi
	fi
else
	exit 0
fi

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
%attr(600,root,root) /etc/psme/psme-network-configuration.json
%attr(600,root,root) /etc/psme/bdc-r-network-configuration.json
%attr(700,root,root) %{_bindir}/psme-network
%attr(700,root,root) /etc/psme/enp0s20f0-vlans-create
%attr(700,root,root) /etc/psme/enp0s20f0-vlans-delete
/etc/systemd/network/*
/usr/lib/systemd/system/*
/usr/local/bin/*


%changelog
