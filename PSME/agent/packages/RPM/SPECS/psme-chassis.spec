Summary: Package for RackScale PSME chassis agent
Name: psme-chassis
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp, libmicrohttpd, libcurl, libstdc++, uuid-c++, libgcrypt, openssl, cyMUX >= 1.04-0, libcyusbserial >= 1.0-0
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
cp -p %{SOURCEURL0}/etc/psme/psme-chassis-configuration.json ./etc/psme/

%pre
#if this is not the initial installation, then backup old configuration files
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-chassis-configuration.json ]; then
		cp /etc/psme/psme-chassis-configuration.json /etc/psme/psme-chassis-configuration.json.old
	fi
else
	exit 0
fi

%post
#creates service for psme-chassis
cat <<EOF >/lib/systemd/system/psme-chassis.service
[Unit]
Description=Managing PSME chassis agent
Requires=cyMUX.service
After=network.target network-online.target cyMUX.service

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

systemctl daemon-reload
systemctl enable psme-chassis

%preun
systemctl stop psme-chassis

%postun
#if this is uninstallation, then remove services and configuration files
if [ "$1" == 0 ]; then
	systemctl disable psme-chassis
	if [ -f /etc/systemd/system/psme-chassis.service ]; then
	    rm /etc/systemd/system/psme-chassis.service
	fi
	if [ -f /lib/systemd/system/psme-chassis.service ]; then
	    rm /lib/systemd/system/psme-chassis.service
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(600,root,root) /etc/psme/psme-chassis-configuration.json
%attr(700,root,root) %{_bindir}/psme-chassis

%changelog
