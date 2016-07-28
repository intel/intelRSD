Summary: Package for RackScale PSME network
Name: psme-network-fm10000
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ libnl3 dcrp-protocol >= 1.1.47
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
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-network-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/enp0s20f0-vlans-create ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/enp0s20f0-vlans-delete ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/dcrpini ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/lldpad-conf ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/mesh_ports.sh ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/mesh_vlans.sh ./etc/psme/
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
cp -p %{SOURCEURL0}/usr/lib/systemd/system/lldpad-conf.service ./usr/lib/systemd/system/
cp -p %{SOURCEURL0}/usr/lib/systemd/system/psme-enp0s20f0-vlans.service ./usr/lib/systemd/system/

%post
#DCRP configuration
dcrpserv=/usr/lib/systemd/system/dcrpd.service
dcrpbak=/tmp/dcrpd.service-bak
#Check if DCRP wasn't already configured with the setup script
dcrppre=`awk '/ExecStartPre=/{print "1"}' $dcrpserv`
if [ "$dcrppre" == "1" ] ; then
# the setup script is configured, skip the configuration.
  echo DCRP is already configured.
else
  cp -f $dcrpserv $dcrpbak || exit 1
  # add a new line before ExecStart so the setup script is called before dcrpd
  awk '/ExecStart=/{print "ExecStartPre=/etc/psme/dcrpini"}1' $dcrpbak > $dcrpserv
fi
systemctl daemon-reload
systemctl enable dcrpd
systemctl enable lldpad
systemctl enable lldpad-conf

cat <<EOF >/lib/systemd/system/psme-network.service
[Unit]
Description=Managing PSME network
Wants=fm10kd.service
After=network.target network-online.target psme-enp0s20f0-vlans.service fm10kd.service dcrpd.service

[Service]
Type=simple
EnvironmentFile=/etc/psme/mesh_ports.sh
ExecStart=/usr/bin/psme-network /etc/psme/psme-network-configuration.json
SELinuxContext=system_u:system_r:initrc_t:s0-s0:c0.c1023
PIDFile=/var/run/psme-network.pid

[Install]
WantedBy=multi-user.target
EOF

if [ ! -f /etc/systemd/system/psme-enp0s20f0-vlans.service ]; then
    ln -s /lib/systemd/system/psme-enp0s20f0-vlans.service /etc/systemd/system/psme-enp0s20f0-vlans.service
fi

systemctl daemon-reload
systemctl enable psme-enp0s20f0-vlans

if [ ! -f /etc/systemd/system/psme-network.service ]; then
    ln -s /lib/systemd/system/psme-network.service /etc/systemd/system/psme-network.service
fi
systemctl daemon-reload
systemctl enable psme-network

%preun
systemctl stop psme-network
systemctl stop dcrpd
systemctl stop lldpad-conf
systemctl stop lldpad

%postun
dcrpserv=/usr/lib/systemd/system/dcrpd.service
dcrpbak=/tmp/dcrpd.service-bak
if [ "$1" == 0 ]; then
	systemctl disable psme-network
	if [ -f /etc/systemd/system/psme-network.service ]; then
	    rm /etc/systemd/system/psme-network.service
	fi
	if [ -f /lib/systemd/system/psme-network.service ]; then
	    rm /lib/systemd/system/psme-network.service
	fi
	systemctl disable lldpad
	systemctl disable lldpad-conf
	systemctl disable dcrpd
#check if DCRP was configured with the setup script
	dcrppre=`awk '/ExecStartPre=/{print "1"}' $dcrpserv`
	if [ "$dcrppre" == "1" ] ; then
		cp -f $dcrpserv $dcrpbak || exit 1
#remove the line that was added during PSME installation
		awk '!/ExecStartPre=/' $dcrpbak > $dcrpserv || exit 1
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
%attr(700,root,root) /etc/psme/enp0s20f0-vlans-create
%attr(700,root,root) /etc/psme/enp0s20f0-vlans-delete
%attr(700,root,root) /etc/psme/dcrpini
%attr(700,root,root) /etc/psme/lldpad-conf
%attr(700,root,root) /etc/psme/mesh_ports.sh
%attr(700,root,root) /etc/psme/mesh_vlans.sh
/etc/systemd/network/*
/usr/lib/systemd/system/*


%changelog
