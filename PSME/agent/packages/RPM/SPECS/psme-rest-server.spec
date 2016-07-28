Summary: Package for RackScale PSME REST server with PSME chassis agent
Name: psme-rest-server
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ libgcrypt openssl
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}

%filter_from_requires /libcyusbserial.*/d
%filter_from_requires /libgcrypt.*/d
%filter_setup

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/etc/psme
mkdir -p $RPM_BUILD_ROOT/usr/lib64
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-rest-server-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/psme-chassis-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/chassis_bdc_r_configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/metadata.xml ./etc/psme/
cp -p %{SOURCEURL0}/usr/lib64/libcyusbserial.so.1 ./usr/lib64/
cp -p %{SOURCEURL0}/usr/bin/psme-gen-serv-cert.sh ./usr/bin/

%pre
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-rest-server-configuration.json ]; then
		cp /etc/psme/psme-rest-server-configuration.json /etc/psme/psme-rest-server-configuration.json.old
	fi
	if [ -f /etc/psme/psme-chassis-configuration.json ]; then
		cp /etc/psme/psme-chassis-configuration.json /etc/psme/psme-chassis-configuration.json.old
	fi
	if [ -f /etc/psme/chassis_bdc_r_configuration.json ]; then
		cp /etc/psme/chassis_bdc_r_configuration.json /etc/psme/chassis_bdc_r_configuration.json.old
	fi
	if [ -f /etc/psme/psme-chassis-bdc-a-configuration.json ]; then
		cp /etc/psme/psme-chassis-bdc-a-configuration.json /etc/psme/psme-chassis-bdc-a-configuration.json.old
	fi
else
	exit 0
fi

%post
mkdir -p /etc/psme/certs

if [ -f /etc/ONPSS_VERSION ]; then
	mv /etc/psme/psme-chassis-configuration.json /etc/psme/psme-chassis-bdc-a-configuration.json
	mv /etc/psme/chassis_bdc_r_configuration.json /etc/psme/psme-chassis-configuration.json
fi

#install libcyusbserial.so.1
ln -sf /usr/lib64/libcyusbserial.so.1 /usr/lib64/libcyusbserial.so
ldconfig

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

cat <<EOF >/lib/systemd/system/psme-chassis.service
[Unit]
Description=Managing PSME chassis agent
After=network.target network-online.target

[Service]
Type=simple
ExecStart=/usr/bin/psme-chassis /etc/psme/psme-chassis-configuration.json
PIDFile=/var/run/psme-chassis.pid

[Install]
WantedBy=multi-user.target
EOF
if [ ! -f /etc/systemd/system/psme-chassis.service ]; then
    ln -s /lib/systemd/system/psme-chassis.service /etc/systemd/system/psme-chassis.service
fi

if [ ! -f /etc/ONPSS_VERSION ]; then
	sed -i 's/"enp0s20f0.4094"/"em1"/' /etc/psme/psme-rest-server-configuration.json
fi

systemctl daemon-reload
systemctl enable psme-rest-server
systemctl enable psme-chassis

# Disabel cyreceive service which affects psme-chassis IPMB service
if [ -f /usr/lib/systemd/system/cyreceive.service ]; then
    systemctl stop cyreceive
    systemctl disable cyreceive
fi

%preun
systemctl stop psme-rest-server
systemctl stop psme-chassis
if [ "$1" == 0 ]; then
	if [ -f /etc/ONPSS_VERSION ]; then
		mv /etc/psme/psme-chassis-configuration.json /etc/psme/chassis_bdc_r_configuration.json
		mv /etc/psme/psme-chassis-bdc-a-configuration.json /etc/psme/psme-chassis-configuration.json
	fi
else
	exit 0
fi

%postun
if [ "$1" == 0 ]; then
	systemctl disable psme-rest-server
	systemctl disable psme-chassis
	if [ -f /etc/systemd/system/psme-rest-server.service ]; then
	    rm /etc/systemd/system/psme-rest-server.service
	fi
	if [ -f /lib/systemd/system/psme-rest-server.service ]; then
	    rm /lib/systemd/system/psme-rest-server.service
	fi
	if [ -f /etc/systemd/system/psme-chassis.service ]; then
	    rm /etc/systemd/system/psme-chassis.service
	fi
	if [ -f /lib/systemd/system/psme-chassis.service ]; then
	    rm /lib/systemd/system/psme-chassis.service
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
%attr(600,root,root) /etc/psme/psme-chassis-configuration.json
%attr(600,root,root) /etc/psme/chassis_bdc_r_configuration.json
%attr(600,root,root) /etc/psme/metadata.xml
%attr(700,root,root) %{_bindir}/psme-chassis
%attr(755,root,root) /usr/lib64/libcyusbserial.so.1
%attr(700,root,root) %{_bindir}/psme-gen-serv-cert.sh

%changelog
