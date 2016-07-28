Summary: Package for RackScale PSME compute
Name: psme-compute
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: psme-chassis jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ OpenIPMI-libs
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
cp -p %{SOURCEURL0}/etc/psme/psme-compute-configuration.json ./etc/psme/

%pre
#if this is not the initial installation, then backup old configuration files
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-compute-configuration.json ]; then
		cp /etc/psme/psme-compute-configuration.json /etc/psme/psme-compute-configuration.json.old
	fi
else
	exit 0
fi

%post
#creates service for psme-compute
cat <<EOF >/lib/systemd/system/psme-compute.service
[Unit]
Description=Managing PSME compute
After=network.target network-online.target

[Service]
Type=simple
ExecStart=/usr/bin/psme-compute /etc/psme/psme-compute-configuration.json
PIDFile=/var/run/psme-compute.pid

[Install]
WantedBy=multi-user.target
EOF
if [ ! -f /etc/systemd/system/psme-compute.service ]; then
    ln -s /lib/systemd/system/psme-compute.service /etc/systemd/system/psme-compute.service
fi
systemctl daemon-reload
systemctl enable psme-compute

%preun
systemctl stop psme-compute

%postun
#if this is uninstallation, then remove services
if [ "$1" == 0 ]; then
	systemctl disable psme-compute
	if [ -f /etc/systemd/system/psme-compute.service ]; then
		rm /etc/systemd/system/psme-compute.service
	fi
	if [ -f /lib/systemd/system/psme-compute.service ]; then
		rm /lib/systemd/system/psme-compute.service
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(600,root,root) /etc/psme/psme-compute-configuration.json
%attr(700,root,root) %{_bindir}/psme-compute
%attr(700,root,root) %{_bindir}/encrypt

%changelog
